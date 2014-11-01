//AST.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ASTops.h"
#include"assembler.h"

int scopeF=0;
int scopeOther=0;
int maxScope=0;
int inOther=0;
int ERROR_IN_TYPE=0;
int ERROR_IN_LINE=0;
int scope_temp;
int N_Declare_Error=0;

int rem_PT(terminal t)// check whether to remove from PARSE Tree to get the AST
{
int i;
terminal checkArray[26]={TRUE_, FALSE_, FOR, SWITCH, WHILE, MINUS, BOOLEAN, INTEGER,GET_VALUE, PRINT,ARRAY};
for (i=0;i<26;++i)
{ if (t==checkArray[i])return 0;}
return 1;
}

ASTnode createASTNode(ASTnode parent,  G_element e,  tag t, tokenInfo tokenLink,  IDEnt link,  valueOfASTnode val,  terminal tp, ASTnode prev, FILE * fp)
{
ASTnode temp;

temp=(ASTnode)malloc(sizeof(struct AST_NODE));
temp->scope=-1;
temp->parent=parent;
temp->childQ=createASTQueue();
temp->plist=createPQueue();
temp->t=t;
temp->link=link;
temp->tokenLink=tokenLink;
temp->tp;
temp->next=NULL;
temp->prev=prev;
temp->e=e;
temp->val=val;
return temp;
}

ParentQ createPQueue()
{
ParentQ q;
q.front=NULL;
q.back=NULL;
q.size=0;
return q;
}

PQnode createPQnode(nonTerminal nt)
{
PQnode p;
p=(PQnode)malloc(sizeof(struct PQNode));
p->next=NULL;
p->nt=nt;
return p;
}

ParentQ enPQueue(ParentQ q,nonTerminal nt)
{
PQnode p;
p=createPQnode(nt);
if (q.size==0)
  {q.front=p;
   q.back=q.front;
   }
else
   {q.back->next=p;
    q.back=p;
    if (q.front->next==NULL)
       q.front->next==q.back;
    }
q.size++;
return q;
}


ASTQueue createASTQueue()
{
ASTQueue q;
q.front=NULL;
q.back=NULL;
q.size=0;
return q;
}


int isaop(terminal t)
{int i;
terminal aop[4]={PLUS,MINUS,MUL,DIV};
for (i=0;i<4;i++)
if ((terminal)aop[i]==t)
return 1;
return 0;
}

int islop(terminal t)
{int i;
terminal lop[2]={AND,OR};
for (i=0;i<2;i++)
if ((terminal)lop[i]==t)
return 1;
return 0;
}

int isrelop(terminal t)
{int i;
terminal relop[6]={LT, LE, GE, GT, EQ, NE};
for (i=0;i<6;i++)
if ((terminal)relop[i]==t)
return 1;
return 0;
}



ASTree buildASTree(parseTree PT, hashTable2 h_ID, hashTable2 h_fn,char * outputfile)
{FILE * fp;
fp=fopen(outputfile,"w");
PT.currentNode=PT.root;
ASTree ast;
ast.root=NULL;
ast.currentNode=ast.root;

ast.root=FillQueue(PT.root,ast.root,NULL,NULL,h_ID,h_fn,fp);
ast.currentNode=ast.root;
printASTree(ast,fp);
fclose(fp);
return ast;
}




ASTnode FillNodeAST(pnode p,ASTnode parent,ASTnode prev, hashTable2 h_ID, hashTable2 h_fn, FILE * fp)
{
ASTnode temp;
valueOfASTnode  v;
terminal tp;
findresult2 f;
v.na='n';//n==not applicable
tp=(terminal)-1;
if (p->t==non_term)
  {if  (p->e.nt==op1 || p->e.nt==op2 || p->e.nt==logicalOp || p->e.nt==relationalOp )//(p->child.front->t==term && p->child.front->e.t==e) ||
     temp=NULL;
   else  temp=(ASTnode)createASTNode(parent,p->e,p->t,p->tokenLink,NULL,v,tp,prev,fp);
   }
  //end of if non terminal

else
 {  
    if (p->e.t==ID)
      { 
        if (p->parent->e.nt==moduleDeclaration || p->parent->e.nt==module || p->parent->e.nt==moduleReuseStmt)
         {f=find2(h_fn,p->tokenLink->val.s,scopeF,1);

		scope_temp=scopeF;
		while(f.code==-1&&scope_temp!=0)
		{
			f=find2(h_fn,p->tokenLink->val.s,--scope_temp,1);
		}
          temp=(ASTnode)createASTNode(parent,p->e,p->t,p->tokenLink,f.node,v,(terminal)MODULE,prev,fp);
          temp->scope=scopeF;  
          }
      else if (inOther!=0)
         {f=find2(h_ID,p->tokenLink->val.s,scopeOther,0);
	  scope_temp=scopeOther;
	  while(f.code==-1&&scope_temp!=0)
	{
		f=find2(h_ID,p->tokenLink->val.s,--scope_temp,0);
	}
         if (f.code!=-1)
          	{if (f.node->entity.ivar.v_or_a==0)// (if it is a variable type)
            		{tp=f.node->entity.ivar.var.v.type;
			temp=(ASTnode)createASTNode(parent,p->e,p->t,p->tokenLink,f.node,v,tp,prev,fp);
			temp->scope=scope_temp;}
          	else 
			{tp=f.node->entity.ivar.var.a.type;
			temp=(ASTnode)createASTNode(parent,p->e,p->t,p->tokenLink,f.node,v,tp,prev,fp);
			temp->scope=scope_temp;}
 		}    
          else 
		{temp=(ASTnode)createASTNode(parent,p->e,p->t,p->tokenLink,NULL,v,tp,prev,fp);} 
          
         }
       else 
         {f=find2(h_ID,p->tokenLink->val.s,scopeF,0);
		scope_temp=scopeF;
		while(f.code==-1&&scope_temp!=0)
		{
			f=find2(h_ID,p->tokenLink->val.s,--scope_temp,0);
		}
         if (f.code!=-1)
          	{if (f.node->entity.ivar.v_or_a==0)// (if it is a variable type)
            		{tp=f.node->entity.ivar.var.v.type;
			temp=(ASTnode)createASTNode(parent,p->e,p->t,p->tokenLink,f.node,v,tp,prev,fp);
			temp->scope=scope_temp;}
          	else 
			{tp=f.node->entity.ivar.var.a.type;
			temp=(ASTnode)createASTNode(parent,p->e,p->t,p->tokenLink,f.node,v,tp,prev,fp);
			temp->scope=scope_temp;}
 		}    
          else 
		{temp=(ASTnode)createASTNode(parent,p->e,p->t,p->tokenLink,NULL,v,tp,prev,fp);} 
          
       }  
      }
    else if (p->e.t==NUM)
          { v.num=p->tokenLink->val.i;
            temp=(ASTnode)createASTNode(parent,p->e,p->t,p->tokenLink,NULL,v,(terminal)INTEGER,prev,fp);
            
////
      //printASTnode(temp);
   
           }
   else if (p->e.t==RNUM)
          { v.rnum=p->tokenLink->val.f;
            temp=(ASTnode)createASTNode(parent,p->e,p->t,p->tokenLink,NULL,v,(terminal)REAL,prev,fp);
            return temp;
////
     //printASTnode(temp);AST
            }
               
  else if (rem_PT(p->e.t)==0)
           {if (p->e.t==(terminal)TRUE_ || p->e.t==(terminal)FALSE_)
                  tp=(terminal)BOOLEAN;
            else tp=(terminal)-1;//-1 means no type
            temp=(ASTnode)createASTNode(parent,p->e,p->t,p->tokenLink,NULL,v,tp,prev,fp);
////
          //printASTnode(temp);
           }
   else return NULL;
}// end of terminals

return temp;

}
         

ASTnode FillQueue(pnode p,ASTnode a,ASTnode parent,ASTnode prev,hashTable2 h_ID, hashTable2 h_fn, FILE * fp)
{pnode t;

if (p->t==non_term && (p->e.nt==iterativeStmt || p->e.nt==conditionalStmt))
{inOther++;
maxScope++;
scopeOther=maxScope;
}
else if (p->t==non_term && (p->e.nt==moduleDeclaration || p->e.nt==module || p->e.nt==driverModule))
{
t=(pnode)malloc(sizeof(pnode));
maxScope++;
scopeF=maxScope;
}

a=FillNodeAST(p,parent,prev,h_ID,h_fn,fp);

valueOfASTnode  v;
terminal tp;
v.na='n';//n==not applicable
tp=(terminal)-1;

if (p->t==non_term)
   {if (p->e.nt==assignmentStmt)
     {
     
      ASTnode assign, lchild, rchild;
      //assign=FillNodeAssignOP(p,temp,NULL,h_ID,h_fn) 
      assign=createASTNode(a,(G_element)(terminal)ASSIGNOP,(tag)term,p->child.front->tokenLink,NULL,v,tp,NULL,fp);
      lchild=FillNodeAST(p->child.front,assign,NULL,h_ID,h_fn,fp);
      pnode p1,p2;
      p1=p->child.front->next;
      if (p1->child.front->e.nt==lvalueARRStmt)
         {p2=p1->child.front->child.front->next;
          ASTnode Index;
          Index=FillQueue(p2,Index,assign,lchild,h_ID,h_fn,fp);     
          rchild=FillQueue(p2->next->next->next,rchild,assign,Index,h_ID,h_fn,fp);
      Index->parent=assign;
      lchild->parent=assign; 
      assign->parent=a; 
      assign->childQ=enASTqueue(assign->childQ,lchild);
      assign->childQ=enASTqueue(assign->childQ,Index);
      if (rchild->childQ.size==1 && rchild->childQ.front->t!=term)
         {rchild->childQ.front->parent=assign;
          rchild->childQ.front->plist=enPQueue(rchild->childQ.front->plist,rchild->e.nt);
          assign->childQ=enASTqueue(assign->childQ,rchild->childQ.front);
	  free(rchild);
	 }     
      else
          {rchild->parent=assign;        
           assign->childQ=enASTqueue(assign->childQ,rchild);}
           a->childQ=enASTqueue(a->childQ,assign);
          }
     
      else
      {rchild=FillQueue(p1,rchild,assign,lchild,h_ID,h_fn,fp);
      lchild->parent=assign; 
      assign->parent=a; 
      assign->childQ=enASTqueue(assign->childQ,lchild);
      if (rchild->childQ.size==1 && rchild->childQ.front->t!=term)
         {rchild->childQ.front->parent=assign;
	 rchild->childQ.front->plist=enPQueue(rchild->childQ.front->plist,rchild->e.nt);
         assign->childQ=enASTqueue(assign->childQ,rchild->childQ.front);
	 free(rchild);}     
      else
          {rchild->parent=assign;        
          assign->childQ=enASTqueue(assign->childQ,rchild);}
      	  a->childQ=enASTqueue(a->childQ,assign);
       	  }
    }
       
else if (p->e.nt==arithmeticOrBooleanExpr)
      { 
               
           // arithmaticOrBoolean->anyterm,N7, where N7!=e
            
             ASTnode lchild,rchild,op;
             lchild=FillQueue(p->child.front,lchild,a,NULL,h_ID,h_fn,fp);
             if (lchild->t==non_term && lchild->e.nt==AnyTerm)
                  {pnode k;
                   k=p->child.front->next;
                   rchild=FillQueue(k,rchild,a,lchild,h_ID,h_fn,fp);
                   if (k->child.front->t==non_term)
                       {op=(ASTnode)createASTNode(a,k->child.front->child.front->e,k->child.front->child.front->t,k->child.front->child.front->tokenLink,NULL,v,tp,NULL,fp);
                   	if (lchild->childQ.size==1 && lchild->childQ.front->t!=term)
                             {lchild->childQ.front->parent=op;
			      lchild->childQ.front->plist=enPQueue(lchild->childQ.front->plist,lchild->e.nt);
                              op->childQ=enASTqueue(op->childQ,lchild->childQ.front);
			      free(lchild);}     
                   	else
          		     {lchild->parent=op;        
                              op->childQ=enASTqueue(op->childQ,lchild);}
                   	if (rchild->childQ.size==1 && rchild->childQ.front->t!=term)
                             {rchild->childQ.front->parent=op;
				rchild->childQ.front->plist=enPQueue(rchild->childQ.front->plist,rchild->e.nt);                              
				op->childQ=enASTqueue(op->childQ,rchild->childQ.front);
				free(rchild);}     
                   	else
          		     {rchild->parent=op;        
                              op->childQ=enASTqueue(op->childQ,rchild);}
                        op->parent=a;            
                        a->childQ=enASTqueue(a->childQ,op);             
                        }
                   else  
                      {if (lchild->childQ.size==1 && lchild->childQ.front->t!=term)
                             {lchild->childQ.front->parent=a;
                              lchild->childQ.front->plist=enPQueue(lchild->childQ.front->plist,lchild->e.nt);
                              a->childQ=enASTqueue(a->childQ,lchild->childQ.front);
			      free(lchild);}          
                   	else
          		     {lchild->parent=a;        
                              a->childQ=enASTqueue(a->childQ,lchild);}
                        }
                     }

            else  
                      { if (lchild->childQ.size==1 && lchild->childQ.front->t!=term)
                             {lchild->childQ.front->parent=a;
                              lchild->childQ.front->plist=enPQueue(lchild->childQ.front->plist,lchild->e.nt);
                              a->childQ=enASTqueue(a->childQ,lchild->childQ.front);
			      free(lchild);}          
                   	else
          		     {lchild->parent=a;        
                              a->childQ=enASTqueue(a->childQ,lchild);}
                       }
      }

else if (p->e.nt==N7 || p->e.nt==N8 || p->e.nt==N4 || p->e.nt==N5)
       {
    //// Nx-><op><a><Nx>;Nx->e
         if (p->child.front->t!=term)
           	if (p->child.front->next->next->child.front->t==term) 
               		{t=p->child.front->next;
                	while (t!=NULL)
                	{
                 	ASTnode child;
                 	child=FillQueue(t,child,parent,prev,h_ID,h_fn,fp);
                        if ((child!=NULL) && (!(child->t==non_term && child->childQ.front==NULL)))
                 	{
			if (child->childQ.size==1 && child->childQ.front->t!=term)
                             {child->childQ.front->parent=a;
			      child->childQ.front->plist=enPQueue(child->childQ.front->plist,child->e.nt);
                              a->childQ=enASTqueue(a->childQ,child->childQ.front);
				free(child);}     
                   	else
          		     {child->parent=a;        
                              a->childQ=enASTqueue(a->childQ,child);}
                        }
                  	if (t->t==non_term && (t->e.nt==iterativeStmt || t->e.nt==conditionalStmt))
                  	{inOther--;scopeOther--;}
                  	t=t->next;
                  	if (child!=NULL)
                  	child=child->next; 
                  	}
                	}
           	else
                	{ASTnode lchild,rchild,op;
                	 op=(ASTnode)createASTNode(a,p->child.front->next->next->child.front->child.front->e,p->child.front->next->next->child.front->child.front->t, p->child.front->next->next->child.front->child.front->tokenLink, NULL,v,tp,NULL,fp);
                 	lchild=FillQueue(p->child.front->next,lchild,op,NULL ,h_ID,h_fn,fp);
                 	rchild=FillQueue(p->child.front->next->next,rchild,op,lchild, h_ID,h_fn,fp);
                 
 ////
  //    printASTnode(op);          
			if (lchild->childQ.size==1 && lchild->childQ.front->t!=term)
                             {lchild->childQ.front->parent=op;
				lchild->childQ.front->plist=enPQueue(lchild->childQ.front->plist,lchild->e.nt);
                              op->childQ=enASTqueue(op->childQ,lchild->childQ.front);
				free(lchild);}     
                   	else
          		     {lchild->parent=op;        
                              op->childQ=enASTqueue(op->childQ,lchild);}
                   	if (rchild->childQ.size==1 && rchild->childQ.front->t!=term)
                             {rchild->childQ.front->parent=op;
				rchild->childQ.front->plist=enPQueue(rchild->childQ.front->plist,rchild->e.nt);
                              op->childQ=enASTqueue(op->childQ,rchild->childQ.front);
				free(rchild);}     
                   	else
          		     {rchild->parent=op;        
                              op->childQ=enASTqueue(op->childQ,rchild);}

                 	op->parent=a;
                 	a->childQ=enASTqueue(a->childQ,op);
                	}
          else
		{t=p->child.front;
                while (t!=NULL)
                {
                 ASTnode child;
                 child=FillQueue(t,child,parent,prev,h_ID,h_fn,fp);
                 if ((child!=NULL) && (!(child->t==non_term && child->childQ.front==NULL)))
                       {if (child->childQ.size==1 && child->childQ.front->t!=term)
                             {child->childQ.front->parent=a;
				child->childQ.front->plist=enPQueue(child->childQ.front->plist,child->e.nt);
                              a->childQ=enASTqueue(a->childQ,child->childQ.front);
				free(child);}     
                   	else
          		     {child->parent=a;        
                              a->childQ=enASTqueue(a->childQ,child);}
                        }
                  if (t->t==non_term && (t->e.nt==iterativeStmt || t->e.nt==conditionalStmt))
                  {inOther--;scopeOther--;}
                  t=t->next;
                  if (child!=NULL)
                  child=child->next; 
                  }
                }               
       }

else if (p->e.nt==AnyTerm || p->e.nt==arithmeticExpr || p->e.nt==term_1)
          { 
           if (p->child.front->next->child.front->t==non_term)
             {ASTnode lchild,rchild,op;
             op=(ASTnode)createASTNode(a,p->child.front->next->child.front->child.front->e,p->child.front->next->child.front->child.front->t, p->child.front->next->child.front->child.front->tokenLink ,NULL,v,tp,NULL,fp);
             lchild=FillQueue(p->child.front,lchild,op,NULL, h_ID,h_fn,fp);
             rchild=FillQueue(p->child.front->next,rchild,op,lchild, h_ID,h_fn,fp);
              lchild->parent=op;
              rchild->parent=op;
              op->childQ=enASTqueue(op->childQ,lchild); 
              op->childQ=enASTqueue(op->childQ,rchild);
              op->parent=a;
              a->childQ=enASTqueue(a->childQ,op);             
           
             }
      else 
             {t=p->child.front;
                while (t!=NULL)
                {
                 ASTnode child;
                 child=FillQueue(t,child,parent,prev,h_ID,h_fn,fp);
                 if ((child!=NULL) && (!(child->t==non_term && child->childQ.front==NULL)))
                 	{if (child->childQ.size==1 && child->childQ.front->t!=term)
                             {child->childQ.front->parent=a;
				child->childQ.front->plist=enPQueue(child->childQ.front->plist,child->e.nt);
                              a->childQ=enASTqueue(a->childQ,child->childQ.front);
				free(child);}     
                 	 else
          		     {child->parent=a;        
                              a->childQ=enASTqueue(a->childQ,child);}
                 	 }
                  if (t->t==non_term && (t->e.nt==iterativeStmt || t->e.nt==conditionalStmt))
                  {inOther--;scopeOther--;}
                  t=t->next;
                  if (child!=NULL)
                  child=child->next; 
                  }
              }
           }

else 
       {if (a!=NULL)
          {t=p->child.front;
          prev=NULL;}
       else t=NULL;

       while (t!=NULL)
          {
           ASTnode child;
           child=FillQueue(t,child,parent,prev,h_ID,h_fn,fp);
	   if ((child!=NULL) && (!(child->t==non_term && child->childQ.front==NULL)))
	       		{if (child->childQ.size==1 && child->childQ.front->t!=term)
                             {child->childQ.front->parent=a;
				child->childQ.front->plist=enPQueue(child->childQ.front->plist,child->e.nt);
                              a->childQ=enASTqueue(a->childQ,child->childQ.front);
				free(child);}     
                 	 else
          		     {child->parent=a;        
                              a->childQ=enASTqueue(a->childQ,child);}
                 	 }
           if (t->t==non_term && (t->e.nt==iterativeStmt || t->e.nt==conditionalStmt))
		{inOther--;scopeOther--;}
           t=t->next;
           if (child!=NULL)
                child=child->next; 
          }
       }
}
else
     {if (a!=NULL)
          {t=p->child.front;
          prev=NULL;}
       else t=NULL;

       while (t!=NULL)
          {
           ASTnode child;
           child=FillQueue(t,child,parent,prev,h_ID,h_fn,fp);
	   if ((child!=NULL) && (!(child->t==non_term && child->childQ.front==NULL)))
	       {if (child->childQ.size==1 && child->childQ.front->t!=term)
                             {child->childQ.front->parent=a;
				child->childQ.front->plist=enPQueue(child->childQ.front->plist,child->e.nt);
                              a->childQ=enASTqueue(a->childQ,child->childQ.front);
				free(child);}     
                else
          		     {child->parent=a;        
                              a->childQ=enASTqueue(a->childQ,child);}
                }
           if (t->t==non_term && (t->e.nt==iterativeStmt || t->e.nt==conditionalStmt))
		{inOther--;scopeOther--;}
           t=t->next;
           if (child!=NULL)
                child=child->next; 
          }
       }

return a;
}
 



struct ASTQUEUE enASTqueue(struct ASTQUEUE q, ASTnode p)
{
ASTnode temp;
if (q.size==0)
  {q.front=p;
   q.back=q.front;
   }
else
   {q.back->next=p;
    q.back=p;
    if (q.front->next==NULL)
       q.front->next==q.back;
    }
q.size++;

return q;
}


int ASTQueueTypeChecker(ASTnode a, int ToPrint)
{
ASTnode c;
c=a->childQ.front;

while (c!=NULL && (N_Declare_Error==0))
{
ASTQueueTypeChecker(c, ToPrint);  
c=c->next;
}
a=typeAssignerAndChecker(a, ToPrint);
return ERROR_IN_TYPE;
}


ASTnode typeAssignerAndChecker(ASTnode a, int ToPrint)
{
  if (a->t==term)
     {if (isaop(a->e.t))
      	{if ((a->e.t==MINUS||a->e.t==PLUS) && a->parent->e.nt==expression)
            if (a->next->tp==INTEGER || a->next->tp==REAL)
                 {a->tp=a->next->tp;}
            else 
                 {if (ERROR_IN_LINE==0 && ToPrint==1)
		  printf("\nERROR 6:Type mismatch at line %d",a->tokenLink->lno);
                  ERROR_IN_TYPE=1;ERROR_IN_LINE=1;}
       	 else 
             if ( (a->childQ.front->tp==a->childQ.front->next->tp) && (a->childQ.front->tp==INTEGER || a->childQ.front->tp==REAL))
            	{a->tp=a->childQ.front->tp;}
       	     else 
                {if (ERROR_IN_LINE==0 && ToPrint==1)printf("\nERROR 6:Type mismatch at line %d",a->tokenLink->lno);
		ERROR_IN_TYPE=1;ERROR_IN_LINE=1;}
       }
       else if (isrelop(a->e.t))
       	     if ( (a->childQ.front->tp==a->childQ.front->next->tp) && (a->childQ.front->tp==INTEGER || a->childQ.front->tp==REAL))
               {
                a->tp=(terminal)BOOLEAN;
               }
             else 
               {if (ERROR_IN_LINE==0 && ToPrint==1) printf("\nERROR 6:Type mismatch at line %d",a->tokenLink->lno);
                 ERROR_IN_TYPE=1;ERROR_IN_LINE=1;}
    
      else if  (islop(a->e.t))
            if ( (a->childQ.front->tp==a->childQ.front->next->tp) && (a->childQ.front->tp==BOOLEAN))
              {a->tp=(terminal)BOOLEAN;}
       	    else 
            {if (ERROR_IN_LINE==0 && ToPrint==1)printf("\nERROR 6:Type mismatch at line %d",a->tokenLink->lno);
	     ERROR_IN_TYPE=1;ERROR_IN_LINE=1;}
      else if ((a->e.t==ASSIGNOP)&& (a->parent->e.nt==assignmentStmt) && (a->childQ.front->tp!=a->childQ.back->tp))
             {if (ERROR_IN_LINE==0 && ToPrint==1)
              printf("\nERROR 6:Type mismatch at line %d",a->tokenLink->lno);
              ERROR_IN_TYPE=1;ERROR_IN_LINE=0;}
      else if (a->e.t==NUM)a->tp=INTEGER;
      else if (a->e.t==RNUM)a->tp=REAL;
      else if (a->e.t==ID)
		{if (a->scope!=-1 || a->link!=NULL)
                	if (a->link->entity.ivar.v_or_a==0)
				{a->tp=a->link->entity.ivar.var.v.type;}
                	else 
				{a->tp=a->link->entity.ivar.var.a.type;}
		else
			{N_Declare_Error=1; 
			ERROR_IN_TYPE=1;}
		}
  }
  else
         
       {if (a->e.nt!=program)
		{		
		if ((a->e.nt==statements || a->e.nt==statement || a->e.nt==ioStmt || a->e.nt==iterativeStmt || a->e.nt==simpleStmt || a->e.nt==declareStmt || a->e.nt==conditionalStmt) || (a->parent->e.nt==iterativeStmt && ( a->e.nt==expression || a->e.nt==arithmeticOrBooleanExpr || a->e.nt==AnyTerm || a->e.nt==arithmeticExpr || a->e.nt==factor|| a->e.nt==term_1 )) )
			{N_Declare_Error=0; ERROR_IN_LINE=0;}
		if (a->e.nt==whichStmt || a->e.nt==var || a->e.nt==lvalueIDStmt || a->e.nt==lvalueARRStmt || a->e.nt==index_nt || a->e.nt==expression || a->e.nt==arithmeticOrBooleanExpr || a->e.nt==N7 || a->e.nt==whichID || a->e.nt==AnyTerm || a->e.nt==N8 || a->e.nt==arithmeticExpr || a->e.nt==N4 || a->e.nt==N5 || a->e.nt==factor)
		 	a->tp=a->childQ.front->tp;
		if ((a->e.nt==expression && a->parent->e.nt==iterativeStmt) && (a->tp!=BOOLEAN))
			{if (ERROR_IN_LINE==0 && ToPrint==1)
			 printf("\nERROR 6:Type mismatch at line %d",a->prev->tokenLink->lno);
			 ERROR_IN_TYPE=1;}
        	if ((a->e.nt==whichID) && (a->tp!=INTEGER))
        	        {if (ERROR_IN_LINE==0 && ToPrint==1)
			printf("\nERROR 6:Type mismatch at line %d",a->childQ.front->tokenLink->lno);
			ERROR_IN_TYPE=1;}
		}        
	}
return a;
}



void printASTree(ASTree a, FILE *fp)
{a.currentNode=a.root;

char * s1;char * s5;char * s6;char *s7;
s1=(char *)"CurrentNode";s5=(char*)"Value";s6=(char*)"ParentNode";s7=(char*)"ParentList";
fprintf(fp,"\n%-20s:%-10s:%-25s:%-25s",s1,s5,s6,s7);
printASTreeQueue(fp,a.root);

}

void printASTreeQueue(FILE * fp, ASTnode a)
{int i=0;

while (a!=NULL)
{
printASTnode(fp,a);
if (a->childQ.front!=NULL)
  printASTreeQueue(fp,a->childQ.front);
a=a->next;
}
}

void printASTnode(FILE * fp, ASTnode a)
{
char * p;char * s;char *tp;
char * s8;
s8=(char *)"e";
char * n="---";
if (!(a->parent==NULL))
  if (a->parent->t==term)
    p=getToken(a->parent->e.t);
  else
    p=getNON_term(a->parent->e.nt);
else p=(char*)NULL;

if (a->t==term)
 {s=getToken(a->e.t);}
else s=getNON_term(a->e.nt);
 
if (a->t==term)
   if (a->e.t!=e)
      {    
    
      if (a->e.t==NUM)
             fprintf(fp,"\n%-20s:%-10d:%-25s",s,a->val.num,p);
      else  if (a->e.t==RNUM)
             fprintf(fp,"\n%-20s:%-10f:%-25s",s,a->val.rnum,p);
       
      else if (a->e.t==ID)
              fprintf(fp,"\n%-20s:%-10s:%-25s",s,n,p);
	      
      
       else
               { fprintf(fp,"\n%-20s:%-10s:%-25s",s,n,p);
               }
       }

   else 
       {
    
        fprintf(fp,"\n%-20s:%-10s:%-25s",s8,n,p);}
else

{
 
   fprintf(fp,"\n%-20s:%-10s:%-25s",s,n,p);
}
printPQueue(a->plist,fp);

}

void printPQueue(ParentQ p,FILE * fp)
{
char * s;
PQnode a;
a=p.front;
while (a!=NULL)
{
s=getNON_term(a->nt);
fprintf(fp,":%-25s",s);
a=a->next;
}

}




//////////////////
/*

int main(int argc, char * argv[])
{hashTable h = initializeHashTable(keycount);
	FILE *fp,*ferror;
	ferror=fopen("error.txt","w");
	fclose(ferror);
	fp = fopen("keywords","r");
	keyword k; int c;	
	while(1)
	{
		fscanf(fp,"%d %s",&c,k);
		if (feof(fp))
			break;		
		addKeyword(h, c, k);
	}
	fclose(fp);

                
Grammar G1=(Grammar)malloc(GRAMCONST*sizeof(gHead));
removeComments(argv[1], "clean.txt");
		G1=createGrammar();
 		createFirstSets();
		createFollowSets();
 		createParseTable(G1);
		parseTree Tree;
                Tree=parseInputSourceCode("clean.txt", h, G1);
              
                printParseTree("outfile.txt",Tree);
//populateSymbolTable(argv[1], h,0);
		tokenInfo t = getFirstToken();
		hashTable2 h_ID = initializeHashTable2(13);
		hashTable2 h_fn = initializeHashTable2(13);
		Total_Scope_List scope_tot = (Total_Scope_List) malloc (20* sizeof(struct total_scope_list));
		int j;
		for(j=0;j<20;j++)
		{
			scope_tot[j].scope_start=0;
			scope_tot[j].scope_end=0;
		}
		createIDTable(t,h_fn,h_ID,scope_tot);
		


                ASTree astTree;
                astTree=buildASTree(Tree,h_ID,h_fn,"ASTree.txt");
                ASTQueueTypeChecker(astTree.root);
	//FILE * fp;
        if (ERROR_IN_TYPE==0)
	{fp = fopen("code.asm","w");
	fprintf(fp,".model tiny\n");
	fprintf(fp,".data\n");
	fprintf(fp,"LCHILD DW 8 DUP (?)\nFOR_CTRL DW 8 DUP (?)\nTemp DW ?\nRCHILD DW ?\n");
	generate_data(h_ID,fp);
	fprintf(fp,".code\n.startup\n");
	evaluateQueue(astTree.root,fp); 
	fprintf(fp,".exit\nend");}              
return 1;
}

*/
