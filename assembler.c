
#include "dataDefinition.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include"symboltable.h"
#include"ASTops.h"
#include"assembler.h"

int rel_op=0;
int lch_no=-1;
int for_number=0;
int while_number=0;
int switch_number=0;
int inp=-1,out_var=-1;

void evaluateQueue(ASTnode a,FILE * fp)
{ASTnode c;
if (a->t==non_term)
		if  (a->e.nt==assignmentStmt || a->e.nt==whichStmt || a->e.nt==lvalueIDStmt || a->e.nt==lvalueARRStmt || a->e.nt==index_nt || a->e.nt==expression || a->e.nt==arithmeticOrBooleanExpr || a->e.nt==N7 || a->e.nt==AnyTerm || a->e.nt==N8 || a->e.nt==arithmeticExpr || a->e.nt==N4 || a->e.nt==N5 || a->e.nt==factor|| a->e.nt==term_1)
		       {c=a->childQ.front;
        		while (c!=NULL)
			       	{evaluateQueue(c,fp);
            			c=c->next;
            			}
        		evaluateNode(a,fp);
       			}
   		else if (a->e.nt==var)
         		evaluateNode(a,fp);

		else if(a->e.nt==iterativeStmt||a->e.nt==conditionalStmt)
       			{ 
         		c=a->childQ.front;
         		//while (c!=NULL)
        		//{   
			evaluateQueue(c,fp);
            		//c=c->next;
            		//}
       			}
   		else if(a->e.nt==ioStmt)
			{
			c=a->childQ.front;
			evaluateQueue(c,fp);
			}
   		else
       			{ evaluateNode(a,fp);
         		c=a->childQ.front;
        		while (c!=NULL)
           			{evaluateQueue(c,fp);
            			c=c->next;
            			}
       			}
else if (a->t==term)
//	if (N_Declare_Error==0)
		{if (isaop(a->e.t) || islop(a->e.t) || isrelop(a->e.t)|| a->e.t==ASSIGNOP)
		        {c=a->childQ.front;
		        while (c!=NULL)
           			{evaluateQueue(c,fp);
            			c=c->next;
            			}
        		evaluateNode(a,fp);}
      		else if(a->e.t==FOR)
			{int sc;//scope info of variable
			a=a->next;
			char s[10];
			strcpy(s,a->link->entity.ivar.word);
			sc=a->link->entity.ivar.scope;
			a=a->next->childQ.front;
			int num1 = (int) a->tokenLink->val.i;
			fprintf(fp,"MOV %s%d, %d\n",s,sc,num1);		
			a=a->next;
			int num2 = (int) a->tokenLink->val.i;
			fprintf(fp,"MOV CX, %d\n",num2);
			int this_for=for_number;
			fprintf(fp,"MOV FOR_CTRL[%d], CX\n",this_for*2);
			fprintf(fp,"FOR%d: ",for_number++);
			a=a->parent->next;
			evaluateQueue(a,fp);
			fprintf(fp,"INC %s%d\n",s,sc);
			fprintf(fp,"MOV CX,FOR_CTRL[%d]\n",this_for*2);
			fprintf(fp,"CMP CX, %s%d\n", s,sc);
			fprintf(fp,"JBE FOR%d\n",this_for);
			}
     		else if(a->e.t==WHILE)
			{
			a=a->next;
			int this_while=while_number;
			fprintf(fp,"WHILE%d: ",while_number++);
			evaluateQueue(a,fp);
			fprintf(fp,"MOV CX, AX\n");
			fprintf(fp,"CMP CX, 1\n");
			fprintf(fp,"JNZ END_WHILE%d\n",this_while);
			a=a->next;
			evaluateQueue(a,fp);
			fprintf(fp,"JMP WHILE%d\n",this_while);
			fprintf(fp,"END_WHILE%d: ",this_while);
			}
     		else if(a->e.t==SWITCH)
			{int sc;
			a=a->next;
			char s[10];
			strcpy(s,a->link->entity.ivar.word);
			sc=a->link->entity.ivar.scope;
			fprintf(fp,"MOV CX, %s%d\n",s,sc);
			int this_switch=switch_number;
			switch_number++;
			int case_number=2;
			Symbol sm;
			c=a->next->next;
			if(a->next->e.nt==caseStmts||a->next->e.nt==N9)
				{
				a=a->next->childQ.front;
				sm = a->childQ.front->tokenLink->s;
				if(sm==TRUE_)
					fprintf(fp,"CMP CX, 1\n");
				else if(sm==FALSE_)
					fprintf(fp,"CMP CX, 0\n");
				else 
					fprintf(fp,"CMP CX, %d\n",a->childQ.front->tokenLink->val.i);
				}
			while(a->next->next->e.nt==caseStmts||a->next->next->e.nt==N9)
				{
				fprintf(fp,"JNZ CASE%d%d\n",this_switch,case_number);
				a=a->next;
				evaluateQueue(a,fp);
				fprintf(fp,"JMP END_SWITCH%d\n",this_switch);
				fprintf(fp,"CASE%d%d: ",this_switch,case_number++);
				a=a->next->childQ.front;
				sm = a->childQ.front->tokenLink->s;
				if(sm==TRUE_)
					fprintf(fp,"CMP CX, 1\n");
				else if(sm==FALSE_)
					fprintf(fp,"CMP CX, 0\n");
				else 
					fprintf(fp,"CMP CX, %d\n",a->childQ.front->tokenLink->val.i);
				if(a->next->next==NULL);
					break;
				}
			fprintf(fp,"JNZ DEFAULT%d\n",this_switch);
				a=a->next;
			evaluateQueue(a,fp);
			fprintf(fp,"JMP END_SWITCH%d\n",this_switch);
			//printf("\nc:%d\n",c->e.nt);
			if(c!=NULL)
				{
				fprintf(fp,"DEFAULT%d: ",this_switch);
				evaluateQueue(c,fp);
				}
			else
				{
				fprintf(fp,"DEFAULT%d: \n",this_switch);
				}
			fprintf(fp,"END_SWITCH%d: ",this_switch);
			}
		else if(a->e.t==GET_VALUE)
			{inp++;
			char s[10];
			a=a->next;
			strcpy(s,a->link->entity.ivar.word);
			fprintf(fp,"MOV %s%d,0\nLOOP_IN%d: XOR AX, AX\nMOV AH,1H\nINT 21H\nCMP AL,0AH\nJZ NEXT_IN%d\nSUB AL,30H\nMOV DL, AL\nMOV AL, %s%d\nMOV BL, 10\nMUL BL\nADD AX, DX\nMOV %s%d, AX\nJMP LOOP_IN%d\n NEXT_IN%d:",s,a->link->entity.ivar.scope,inp,inp,s,a->link->entity.ivar.scope,s,a->link->entity.ivar.scope,inp,inp);
			}
		else if(a->e.t==PRINT)
			{out_var++;
			char s[10];
			a=a->next->childQ.front;
			if (a->e.t==NUM)
			{
				fprintf(fp,"MOV BH,0/nMOV Temp, %d\nMOV BL, 10\nLOOP_OUT%d: XOR AX, AX\nXOR DX, DX\nMOV AX, Temp\nMOV AH 0\nDIV BL\nMOV Temp, AL\nMOV DL, AH\nADD DL ,30H\nMOV OUT[BH], DL\nADD BH, 2\nCMP AL, 0\nJZ NEXT_OUT%d\nJMP LOOP_OUT%d\nNEXT_OUT%d:MOV BL,0\nOUT_CONT%d: MOV DL,OUT[BL]\nMOV AH, 2H\nINT 21H\nADD BL, 2\nCMP BH, BL\n JB OUT_CONT%d:",a->tokenLink->val.i,out_var,out_var,out_var,out_var,out_var,out_var);	
						
			}
			else if (a->e.t==RNUM)
			{//RNUM IMPLEMENTATION NOT ASKED
			}
			else 
			{       int sc;
				char s[12];
				strcpy(s,a->link->entity.ivar.word);
				sc=a->link->entity.ivar.scope;
				if(a->next!=NULL)
				{	
					a=a->next->childQ.front;
					char s2[12];//
					strcpy(s2,a->link->entity.ivar.word);
					fprintf(fp,"MOV SI, %s\n",s2);
					fprintf(fp,"MOV BH,0/nMOV Temp, %s%d[SI]\nMOV BL, 10\nLOOP_OUT%d: XOR AX, AX\nXOR DX, DX\nMOV AX, Temp\nMOV AH 0\nDIV BL\nMOV Temp, AL\nMOV DL, AH\nADD DL ,30H\nMOV OUT[BH], DL\nADD BH, 2\nCMP AL, 0\nJZ NEXT_OUT%d\nJMP LOOP_OUT%d\nNEXT_OUT%d:MOV BL,0\nOUT_CONT%d: MOV DL,OUT[BL]\nMOV AH, 2H\nINT 21H\nADD BL, 2\nCMP BH, BL\n JB OUT_CONT%d:",s,sc,out_var,out_var,out_var,out_var,out_var,out_var);
				}
				else
				{
					fprintf(fp,"MOV BH,0/nMOV Temp, %s%d\nMOV BL, 10\nLOOP_OUT%d: XOR AX, AX\nXOR DX, DX\nMOV AX, Temp\nMOV AH 0\nDIV BL\nMOV Temp, AL\nMOV DL, AH\nADD DL ,30H\nMOV OUT[BH], DL\nADD BH, 2\nCMP AL, 0\nJZ NEXT_OUT%d\nJMP LOOP_OUT%d\nNEXT_OUT%d:MOV BL,0\nOUT_CONT%d: MOV DL,OUT[BL]\nMOV AH, 2H\nINT 21H\nADD BL, 2\nCMP BH, BL\n JB OUT_CONT%d:",s,sc,out_var,out_var,out_var,out_var,out_var,out_var);
				}
				
			}	
		}
        }
 



return;
}

void evaluateNode(ASTnode a, FILE * fp)
{

int Vi;
float Vf;
char * value;
char * in;
if (a->t==non_term)
  { if (a->e.nt==var)
        {if (a->childQ.front->e.t==ID)
            if (a->childQ.front->next==NULL)
               {value=a->childQ.front->tokenLink->val.s;
                fprintf(fp,"MOV AX, %s%d\n",value,a->childQ.front->link->entity.ivar.scope);}
            else 
               {value=a->childQ.front->tokenLink->val.s;
               in=(char*)a->childQ.front->next->childQ.front->tokenLink->val.s;
                fprintf(fp,"MOV BL, %s%d\nSUB BL,1\nMOV AL, 2\nMUL BL\nMOV BX, AX\nMOV AX, %s%d[BX]\n",in,a->childQ.front->next->childQ.front->link->entity.ivar.scope,value,a->childQ.front->link->entity.ivar.scope);
               }
       else if (a->childQ.front->e.t==NUM)
             {Vi=a->childQ.front->tokenLink->val.i;
             fprintf(fp,"MOV AX, %d\n",Vi);}       
       else 
             {Vf=a->childQ.front->tokenLink->val.f;
             fprintf(fp,"MOV AX, %f\n",Vf);}
       ASTnode p;
       p=a->parent;
       if (p->t==non_term)
       		{if (p->e.nt==factor || p->e.nt==arithmeticExpr || p->e.nt==term_1 || p->e.nt==AnyTerm || a->e.nt==arithmeticOrBooleanExpr )
        
    	    		{
    	         		lch_no++;
    	         		fprintf(fp,"MOV LCHILD[%d],AX\n",lch_no*2);
    	         
    	    		} 
       		else if (p->e.nt==N4 || p->e.nt==N5 || p->e.nt==N7 || p->e.nt==N8 ) 
             		fprintf(fp,"MOV RCHILD, AX\n");
		    
  		 }
      if (p->t==term)
               {//if (p->e.nt==ASSIGNOP)
                  //      fprintf(fp,"MOV Temp,AX\n"); 
//                else
		if (p->e.nt!=ASSIGNOP) 
			 if (p->childQ.front==a)
				{
    	        	 	lch_no++;
    	        	 	fprintf(fp,"MOV LCHILD[%d],AX\n",lch_no*2);
 	    			} 
			else 
             			fprintf(fp,"MOV RCHILD, AX\n");
		}       
	}
   else if (!(a->childQ.front->t==non_term && a->childQ.front->e.nt==var) && (a->e.nt==factor || a->e.nt==arithmeticExpr || a->e.nt==arithmeticOrBooleanExpr || a->e.nt==term_1 || a->e.nt==AnyTerm))
        
        { ASTnode p;
	  p=a->parent;
          if (p->t==term)
               { //if (p->e.t==ASSIGNOP)
                   //     fprintf(fp,"MOV Temp,AX\n"); 
//                else
		if (p->e.t!=ASSIGNOP) 
			 if (p->childQ.front==a) 
				{
	    	         		lch_no++;
	    	         		fprintf(fp,"MOV LCHILD[%d],AX\n",lch_no*2);
	    	         
	    	    		} 
			else 
	             		fprintf(fp,"MOV RCHILD, AX\n");
		}
	else
		{//if (p->e.nt==iterativeStmt)
			//fprintf(fp,"MOV Temp,AX\n");
		//else
                if ((p->e.nt==N4 || p->e.nt==N5 || p->e.nt==N7 || p->e.nt==N8 ) && p->e.nt!=iterativeStmt)
			fprintf(fp,"MOV RCHILD, AX\n");		  
		}
        } 
             
   else if ( ( (a->childQ.front->t==term) && ( isaop(a->childQ.front->e.t) ) ) && (a->e.nt==N4 || a->e.nt==N5 || a->e.nt==N7 || a->e.nt==N8 ))
			fprintf(fp,"MOV RCHILD, AX\n");		        
            
   else if (a->e.nt==expression)
      {if (a->childQ.front->t==term)
           {if (a->childQ.front->e.t==MINUS)
                fprintf(fp,"MOV AX, -1\nIMUL Temp\n");}
       
       }
}
else 
    {if (a->e.t==PLUS && a->parent->e.nt!=expression)
       {fprintf(fp,"MOV AX,LCHILD[%d]\nADD AX,RCHILD\n",lch_no*2);
         lch_no--;}        
    else if (a->e.t==MINUS && a->parent->e.nt!=expression)
       {fprintf(fp,"MOV AX,LCHILD[%d]\nSUB AX,RCHILD\n",lch_no*2);
         lch_no--;}
    else if (a->e.t==MUL)
       {fprintf(fp,"MOV AX,LCHILD[%d]\nIMUL RCHILD\n",lch_no*2);
         lch_no--;}
    else if (a->e.t==DIV)
       {fprintf(fp,"MOV AX,LCHILD[%d]\nCWD\nIDIV RCHILD\n",lch_no*2);
         lch_no--;}
    else if (a->e.t==GT)
       {fprintf(fp,"MOV AX,LCHILD[%d]\nCMP AX,RCHILD\nJG TRUE%d\nMOV AX,0\nJMP CONT%d\nTRUE%d: MOV AX,1\nCONT%d:",lch_no*2, rel_op, rel_op, rel_op, rel_op);
        rel_op++;lch_no--;
       }
    else if (a->e.t==GE)
       {fprintf(fp,"MOV AX,LCHILD[%d]\nCMP AX,RCHILD\nJGE TRUE%d\nMOV AX,0\nJMP CONT%d\nTRUE%d: MOV AX,1\nCONT%d:",lch_no*2, rel_op, rel_op, rel_op, rel_op);
        rel_op++;lch_no--;
       }
    else if (a->e.t==LT)
       {fprintf(fp,"MOV AX,LCHILD[%d]\nCMP AX,RCHILD\nJL TRUE%d\nMOV AX,0\nJMP CONT%d\nTRUE%d: MOV AX,1\nCONT%d:",lch_no*2, rel_op, rel_op, rel_op, rel_op);
        rel_op++;lch_no--;
       }
   else if (a->e.t==LE)
       {fprintf(fp,"MOV AX,LCHILD[%d]\nCMP AX,RCHILD\nJLE TRUE%d\nMOV AX,0\nJMP CONT%d\nTRUE%d: MOV AX,1\nCONT%d:",lch_no*2, rel_op, rel_op, rel_op, rel_op);
        rel_op++;lch_no--;
       }
   else if (a->e.t==EQ)
       {fprintf(fp,"MOV AX,LCHILD[%d]\nCMP AX,RCHILD\nJE TRUE%d\nMOV AX,0\nJMP CONT%d\nTRUE%d: MOV AX,1\nCONT%d:",lch_no*2, rel_op, rel_op, rel_op, rel_op);
        rel_op++;lch_no--;
       }
   else if (a->e.t==NE)
       {fprintf(fp,"MOV AX,LCHILD[%d]\nCMP AX,RCHILD\nJNE TRUE%d\nMOV AX,0\nJMP CONT%d\nTRUE%d: MOV AX,1\nCONT%d:",lch_no*2, rel_op, rel_op, rel_op, rel_op);
        rel_op++;lch_no--;
       }
    else if (a->e.t==AND)
       {fprintf(fp,"MOV AX,LCHILD[%d]\nAND AX,RCHILD\n",lch_no*2);
       lch_no--;}
     else if (a->e.t==OR)
       {fprintf(fp,"MOV AX,LCHILD[%d]\nOR AX,RCHILD\n",lch_no*2);
       lch_no--;}
      else if (a->e.t==ASSIGNOP)
         {if (a->childQ.size==2)
                    {value=a->childQ.front->tokenLink->val.s;
                      fprintf(fp,"MOV %s%d, AX\n",value,a->childQ.front->link->entity.ivar.scope);}
          else
                    {value=a->childQ.front->tokenLink->val.s;
                     if (a->childQ.front->next->childQ.front->e.t==NUM)
                     	{Vi=a->childQ.front->next->childQ.front->tokenLink->val.i;
                     	fprintf(fp,"MOV %s%d[%d], AX\n",value,a->childQ.front->link->entity.ivar.scope,(Vi-1)*2);}
                     else
		     	{in=(char*)a->childQ.front->next->childQ.front->tokenLink->val.s;
                	fprintf(fp,"MOV TEMP, AX\nMOV AL, 2\nMOV BL, %s%d\nSUB BL,1\nMUL BL\nMOV BX, AX\nMOV AX, Temp\nMOV %s%d[BX],AX\n",in,a->childQ.front->next->childQ.front->link->entity.ivar.scope,value,a->childQ.front->link->entity.ivar.scope);}
                     
                     }
          
           }
}
}


void generate_data(hashTable2 h, FILE *fp)
{
	int index=0;
	while (index < 13)
	{
		chain2 temp = h[index].start;
		if(h[index].flag==occupied)		
		{	
			if(h[index].ID->entity.ivar.v_or_a==0)	
			{
				if(h[index].ID->entity.ivar.var.v.type==REAL)
					fprintf(fp,"%s%d REAL4 ?\n",h[index].ID->entity.ivar.word,h[index].ID->entity.ivar.scope);
				else if(h[index].ID->entity.ivar.var.v.type==INTEGER)
					fprintf(fp,"%s%d DW ?\n",h[index].ID->entity.ivar.word,h[index].ID->entity.ivar.scope);
				else
					fprintf(fp,"%s%d DB ?\n",h[index].ID->entity.ivar.word,h[index].ID->entity.ivar.scope);
			}
			else
			{
				if(h[index].ID->entity.ivar.var.a.type==REAL)
					fprintf(fp,"%s%d REAL4 %d DUP (?)\n",h[index].ID->entity.ivar.word,h[index].ID->entity.ivar.scope,(h[index].ID->entity.ivar.var.a.range_f-h[index].ID->entity.ivar.var.a.range_s+1));
				else if(h[index].ID->entity.ivar.var.a.type==INTEGER)
					fprintf(fp,"%s%d DW ? %d DUP (?)\n",h[index].ID->entity.ivar.word, h[index].ID->entity.ivar.scope,(h[index].ID->entity.ivar.var.a.range_f-h[index].ID->entity.ivar.var.a.range_s+1));
				else
					fprintf(fp,"%s%d DB ? %d DUP (?)\n",h[index].ID->entity.ivar.word, h[index].ID->entity.ivar.scope, (h[index].ID->entity.ivar.var.a.range_f-h[index].ID->entity.ivar.var.a.range_s+1));
			}
			while(temp!=NULL)
			{
				if(temp->ID->entity.ivar.v_or_a==0)
				{
					if(temp->ID->entity.ivar.var.v.type==REAL)
						fprintf(fp,"%s%d REAL4 ?\n",temp->ID->entity.ivar.word,temp->ID->entity.ivar.scope);
					else if(temp->ID->entity.ivar.var.v.type==INTEGER)
						fprintf(fp,"%s%d DW ?\n",temp->ID->entity.ivar.word,temp->ID->entity.ivar.scope);
					else
						fprintf(fp,"%s%d DB ?\n",temp->ID->entity.ivar.word,temp->ID->entity.ivar.scope);
				}
				else
				{
					if(temp->ID->entity.ivar.var.a.type==REAL)
						fprintf(fp,"%s%d REAL4 %d DUP (?)\n",temp->ID->entity.ivar.word,temp->ID->entity.ivar.scope,(temp->ID->entity.ivar.var.a.range_f-temp->ID->entity.ivar.var.a.range_s+1));
					else if(temp->ID->entity.ivar.var.a.type==INTEGER)
						fprintf(fp,"%s%d DW ? %d DUP (?)\n",temp->ID->entity.ivar.word,temp->ID->entity.ivar.scope,(temp->ID->entity.ivar.var.a.range_f-temp->ID->entity.ivar.var.a.range_s+1));
					else
						fprintf(fp,"%s%d DB ? %d DUP (?)\n",temp->ID->entity.ivar.word,temp->ID->entity.ivar.scope,(temp->ID->entity.ivar.var.a.range_f-temp->ID->entity.ivar.var.a.range_s+1));
				}
				temp=temp->next;
			}
		}	
		index ++;
	}
}

