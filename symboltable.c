#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include"symboltable.h"


static int id_num=0;




void createIDTable (tokenInfo tok,hashTable2 h_fn, hashTable2 h_ID, Total_Scope_List scope_tot,int print)
{
	findresult2 result;
	Scope_Chain t = (Scope_Chain) malloc (sizeof (struct scope_chain));
	Scope_Chain SCOPE = (Scope_Chain) malloc (sizeof (struct scope_chain));;
	SCOPE->scope=0;
	SCOPE->next=NULL;
	SCOPE->prev=NULL;
	SCOPE->in_switch=0;
	SCOPE->in_loop=0;
	int univ_scope=0;
	char current_func[10]; 
	strcpy(current_func," ");
	char name;
	int i=-2;
	int default_flag=0;
	int special_scope = 100;
	int switch_type;
	IDEnt for_start = NULL;
	int switch_error[3]={-1,-1,-1};	
	int switch_scope_end;
	int switch_scope_start;
	
if (print==1)//CODE DUPLICATED TO TAKE CARE OF PRINTING ERRORS TO CONSOLE ONLY IN CERTAIN CASES 
{
	while(tok!=NULL)
	{
		if(tok->s == MODULE)
		{
			result=find2(h_fn, tok->n->val.s, SCOPE->scope, 1);	
			if(tok->prev->s!=USE)
			{
				if(tok->prev->s==DEF)
				{
					strcpy(current_func,tok->n->val.s);
					if(result.code!=-1)
						if(result.node->entity.ifn.status==declared&&result.node->entity.ifn.used==0)
						{
							printf("\nERROR 21: Redundant declaration of module \"%s\" ",current_func);
			 				semantic_status=-1;
						}
				}	
				if(result.code==-1)			
				{
					tok=tok->n;			
					IDEnt temp = (IDEnt) malloc (sizeof(struct idEnt));
					strcpy(temp->entity.ifn.word,tok->val.s);
					if(tok->prev->prev->s==DECLARE)
						temp->entity.ifn.status=declared;
					else
					{
						temp->entity.ifn.scope=SCOPE->scope;
						temp->entity.ifn.lno=tok->lno;
						temp->entity.ifn.status=defined;
					}
					addKeyword2(h_fn,temp,1);
				}
				else
				{
					if(tok->prev->s==DEF)
					{	tok=tok->n;
						if(result.node->entity.ifn.status==declared)
						{
							result.node->entity.ifn.scope=SCOPE->scope;
							result.node->entity.ifn.lno=tok->lno;
							result.node->entity.ifn.status=defined;
						}
						else
						{
						printf("\nERROR 19: Overloading of module \"%s\" at line number %d not allowed",current_func,tok->lno);
			 			semantic_status=-1;
						}
					}
					else if(tok->prev->s==DECLARE)
					{
						tok=tok->n;
						FILE *fp;
						printf("\nERROR 21: Redundant declaration of module \"%s\" at line number %d",current_func,tok->lno);
			 			semantic_status=-1;
					}
				}
			}
			else
			{
				tok=tok->n;
				if(strcmp(current_func,tok->val.s)==0)
				{
					printf("\nERROR 23: Function \"%s\" at line number %d cannot be invoked recursively",current_func,tok->lno);
			 		semantic_status=-1;
				}			
				else if(result.code==-1)				
				{	
					printf("\nERROR 20: Undeclared module \"%s\" at line number %d",tok->val.s,tok->lno);
		 			semantic_status=-1;
				}
				else
				{
					result.node->entity.ifn.used=1;
				}		
			}
		}		
		else if(tok->s == INPUT)
		{
			tok=tok->n->n;
			while (tok->s!=SQBC)
			{
				if(tok->s==COMMA)
					tok=tok->n;
				IDEnt temp = (IDEnt) malloc (sizeof(struct idEnt));
				strcpy(temp->entity.ivar.word,tok->val.s);
				temp->entity.ivar.code=id_num++;
				temp->entity.ivar.lno=tok->lno;
				tok=tok->n->n;
				if(tok->s==ARRAY)
				{
					tok=tok->n->n;
					temp->entity.ivar.var.a.range_s=tok->val.i;
					tok=tok->n->n;
					temp->entity.ivar.var.a.range_f=tok->val.i;
					tok=tok->n->n->n;
					temp->entity.ivar.var.a.type=tok->s;
					temp->entity.ivar.w=assign_width(tok->s);
					temp->entity.ivar.v_or_a=1;
				}
				else
				{
					temp->entity.ivar.var.v.type=tok->s;
					temp->entity.ivar.w=assign_width(tok->s);
					temp->entity.ivar.v_or_a=0;
				}
				temp->entity.ivar.scope=SCOPE->scope;
				result=find2(h_ID,temp->entity.ivar.word,SCOPE->scope,0);
				if(result.code>=0)
				{
					printf("\nERROR 8: Repeat declaration of identifier \"%s\" at line number %d",temp->entity.ivar.word,temp->entity.ivar.lno);
			 		semantic_status=-1;
				}
				else
				{	
					addKeyword2(h_ID,temp,0);
					result=find2(h_fn,current_func,SCOPE->scope,1);
					if(result.node->entity.ifn.i_start==NULL)
					{
						Parameter temp_par= (Parameter) malloc (sizeof (struct parameter));
						if(temp->entity.ivar.v_or_a==0)
						{
							temp_par->v_or_a=0;
							temp_par->var.v.type=temp->entity.ivar.var.v.type;
							temp_par->next=NULL;
						}
						else
						{
							temp_par->v_or_a=1;
							temp_par->var.a.type=temp->entity.ivar.var.a.type;
							temp_par->var.a.range_s=temp->entity.ivar.var.a.range_s;
							temp_par->var.a.range_f=temp->entity.ivar.var.a.range_f;
							temp_par->next=NULL;
						}
						result.node->entity.ifn.i_start=temp_par;
					}
					else
					{
						Parameter temp_par= result.node->entity.ifn.i_start;
						while(temp_par->next!=NULL)
							temp_par=temp_par->next;
						temp_par->next=(Parameter) malloc (sizeof (struct parameter));
						if(temp->entity.ivar.v_or_a==0)
						{
							temp_par->next->v_or_a=0;
							temp_par->next->var.v.type=temp->entity.ivar.var.v.type;
							temp_par->next->next=NULL;
						}
						else
						{
							temp_par->next->v_or_a=1;
							temp_par->next->var.a.type=temp->entity.ivar.var.a.type;
							temp_par->next->var.a.range_s=temp->entity.ivar.var.a.range_s;
							temp_par->next->var.a.range_f=temp->entity.ivar.var.a.range_f;
							temp_par->next->next=NULL;
						}
					}	
				}
				tok=tok->n;
			}
		
		}
		else if(tok->s == RETURNS)
		{
			tok=tok->n->n;
			while (tok->s!=SQBC)
			{
				if(tok->s==COMMA)
					tok=tok->n;
				IDEnt temp = (IDEnt) malloc (sizeof(struct idEnt));
				strcpy(temp->entity.ivar.word,tok->val.s);
				temp->entity.ivar.code=id_num++;
				temp->entity.ivar.lno=tok->lno;
				tok=tok->n->n;
				temp->entity.ivar.var.v.type=tok->s;
				temp->entity.ivar.w=assign_width(tok->s);
				temp->entity.ivar.v_or_a=0;
				temp->entity.ivar.scope=SCOPE->scope;
				result=find2(h_ID,temp->entity.ivar.word,SCOPE->scope,0);
				if(result.code>=0)
				{
					printf("\nERROR 8: Repeat declaration of identifier \"%s\" at line number %d",temp->entity.ivar.word,temp->entity.ivar.lno);
			 		semantic_status=-1;
				}
				else
				{	
					addKeyword2(h_ID,temp,0);
					int invalid=check_module_return(SCOPE->scope,tok,h_ID,temp);
					if(invalid==1)
					{					
						printf("\nERROR 12: The variable \"%s\" returned by the function \"%s\" is not assigned any value within the function definition",temp->entity.ivar.word,current_func);
						semantic_status=-1;
					}
					result=find2(h_fn,current_func,SCOPE->scope,1);
					if(result.node->entity.ifn.o_start==NULL)
					{
						Parameter temp_par= (Parameter) malloc (sizeof (struct parameter));
						if(temp->entity.ivar.v_or_a==0)
						{
							temp_par->v_or_a=0;
							temp_par->var.v.type=temp->entity.ivar.var.v.type;
							temp_par->next=NULL;
						}
						else
						{
							temp_par->v_or_a=1;
							temp_par->var.a.type=temp->entity.ivar.var.a.type;
							temp_par->var.a.range_s=temp->entity.ivar.var.a.range_s;
							temp_par->var.a.range_f=temp->entity.ivar.var.a.range_f;
							temp_par->next=NULL;
						}
						result.node->entity.ifn.o_start=temp_par;
					}
					else
					{
						Parameter temp_par= result.node->entity.ifn.o_start;
						while(temp_par->next!=NULL)
							temp_par=temp_par->next;
						temp_par->next=(Parameter) malloc (sizeof (struct parameter));
						if(temp->entity.ivar.v_or_a==0)
						{
							temp_par->next->v_or_a=0;
							temp_par->next->var.v.type=temp->entity.ivar.var.v.type;
							temp_par->next->next=NULL;
						}
						else
						{
							temp_par->next->v_or_a=1;
							temp_par->next->var.a.type=temp->entity.ivar.var.a.type;
							temp_par->next->var.a.range_s=temp->entity.ivar.var.a.range_s;
							temp_par->next->var.a.range_f=temp->entity.ivar.var.a.range_f;
							temp_par->next->next=NULL;
						}
					}
				}
				tok=tok->n;
			}
		}
		else if(tok->s == DECLARE)
		{
			Symbol type; int V_or_A;int range_s;int range_f;
			if(tok->n->s!=MODULE)
			{
				tok=tok->n;
				IDEnt temp = (IDEnt) malloc (sizeof(struct idEnt));
				strcpy(temp->entity.ivar.word,tok->val.s);
				temp->entity.ivar.code=id_num++;
				temp->entity.ivar.lno=tok->lno;
					while(tok->s!=COLON)
						{tok=tok->n;i++;}
					tok=tok->n;
					if(tok->s==ARRAY)
					{
						tok=tok->n->n;
						temp->entity.ivar.var.a.range_s=tok->val.i;
						range_s=tok->val.i;
						tok=tok->n->n;
						temp->entity.ivar.var.a.range_f=tok->val.i;
						range_f=tok->val.i;
						tok=tok->n->n->n;
						temp->entity.ivar.var.a.type=tok->s;
						type=tok->s;
						temp->entity.ivar.w=assign_width(type);
						temp->entity.ivar.v_or_a=1;
						V_or_A=1;
					}
					else
					{
						temp->entity.ivar.var.v.type=tok->s;
						type=tok->s;
						temp->entity.ivar.w=assign_width(type);
						temp->entity.ivar.v_or_a=0;
						V_or_A=0;
					}
					tok=tok->prev;
					while(i>0)
						{tok=tok->prev;i--;}
					temp->entity.ivar.scope=SCOPE->scope;
					result=find2(h_ID,temp->entity.ivar.word,SCOPE->scope,0);
					if(result.code>=0)
					{
						printf("\nERROR 8: Repeat declaration of identifier \"%s\" at line number %d",temp->entity.ivar.word,temp->entity.ivar.lno);
			 			semantic_status=-1;
					}
					else
						{addKeyword2(h_ID,temp,0);}		
					if(i==0)
					{while(tok->s!=COLON)
					{
						if(tok->s!=COMMA)
						{	
							IDEnt temp = (IDEnt) malloc (sizeof(struct idEnt));					
							strcpy(temp->entity.ivar.word,tok->val.s);
							if(V_or_A==1)
							{
								temp->entity.ivar.var.a.range_s=range_s;
								temp->entity.ivar.var.a.range_f=range_f;
								temp->entity.ivar.var.a.type=type;
								temp->entity.ivar.w=assign_width(type);
								temp->entity.ivar.v_or_a=1;
							}
							else
							{
								temp->entity.ivar.var.v.type=type;
								temp->entity.ivar.w=assign_width(type);
								temp->entity.ivar.v_or_a=0;
							}		
							temp->entity.ivar.code=id_num++;
							temp->entity.ivar.scope=SCOPE->scope;
							temp->entity.ivar.lno=tok->lno;
							result=find2(h_ID,temp->entity.ivar.word,SCOPE->scope,0);
							if(result.code>=0)
							{
								printf("\nERROR 8: Repeat declaration of identifier \"%s\" at line number %d",temp->entity.ivar.word,temp->entity.ivar.lno);
			 					semantic_status=-1;
							}
							else
							{addKeyword2(h_ID,temp,0);}
						}
						tok=tok->n;
					}
					}
			}
		i=-2;
		}
		else if(tok->s==ID)
		{
			t = SCOPE;
			while(1)
			{	
				result=find2(h_ID,tok->val.s,t->scope,0);
				if(result.code!=-1)
					break;				
				else
				{
					if(t->prev!=NULL)
						t=(t->prev);
					else
						break;
				}
			}
			
			if(result.code==-1)
			{
				printf("\nERROR 7: Undeclared variable \"%s\" at line number %d",tok->val.s,tok->lno);
			 	semantic_status=-1;
			}	
		}
		else if(tok->s==DEF)
		{
			SCOPE->scope=++univ_scope;
		}
		else if(tok->s==DRIVERDEF)
		{
			SCOPE->scope=++univ_scope;
			IDEnt temp = (IDEnt) malloc (sizeof(struct idEnt));
			strcpy(temp->entity.ifn.word,"program");
			temp->entity.ifn.status=defined;
			temp->entity.ifn.scope=SCOPE->scope;
			temp->entity.ifn.lno=tok->lno;
			addKeyword2(h_fn,temp,1);			
			strcpy(current_func,"program");
		}
		else if(tok->s==SWITCH)
		{
			switch_scope_start=(tok->lno)+1;
					
			Scope_Chain t = (Scope_Chain) malloc (sizeof (struct scope_chain));
					
			t->scope = ++univ_scope;
			
			t->next=NULL;
			t->prev=SCOPE;
			t->in_switch=0;
			t->in_loop=0;
			SCOPE->next=t;
			SCOPE=(SCOPE->next);

			tok->df=SCOPE->scope;	
			tok=tok->n->n;
			t = SCOPE->prev;
			
			while(1)
			{	
				result=find2(h_ID,tok->val.s,t->scope,0);
				if(result.code!=-1)
					break;				
				else
				{
					if(t->prev!=NULL)
						t=(t->prev);
					else
						break;
				}
			}
			if(result.code==-1)
			{
				printf("\nERROR 7: Undeclared variable \"%s\" at line number %d",tok->val.s,tok->lno);
			 	semantic_status=-1;
			}
			else
			{
				switch_type=result.node->entity.ivar.var.v.type;
				if(switch_type==REAL)
				{
					printf("\nERROR 16: Line %d has a switch statement with an identifier of type real ",tok->lno);
			 		semantic_status=-1;
				}
				SCOPE->in_switch=1;
			}
		}
		else if(tok->s==CASE&&SCOPE->in_switch==1)
		{
			tok=tok->n;
			int case_type= tok->s;
			if(case_type!=NUM&&switch_type==INTEGER)
			{
				switch_error[0]=1;
			}
			else if(switch_type==BOOLEAN&&(!(case_type==TRUE_||case_type==FALSE_)))
			{
				switch_error[1]=1;
			}
		}
		else if(tok->s==DEFAULT&&SCOPE->in_switch==1)
		{
			if(switch_type==BOOLEAN)
			{			
				switch_error[2]=1;
			}
			default_flag=1;
		}
		else if(tok->s==END)
		{	
			scope_tot[SCOPE->scope].scope_end=tok->prev->lno;
			tok->df=SCOPE->scope;
			if((SCOPE->in_switch)==1)
			{					
				switch_scope_end = tok->lno;	
				if(default_flag==0&&switch_type!=BOOLEAN)
				{
					printf("\nERROR 15: The case statements lines <%d> to <%d> must have a default statement.",switch_scope_start,switch_scope_end);
			 		semantic_status=-1;
				}
				default_flag=0;
				if(switch_error[0]==1)
				{
					printf("\nERROR 14:  The switch statement lines <%d>  to <%d>  cannot have case statement with case keyword followed by any other value other than  an integer ",switch_scope_start,switch_scope_end);
			 		semantic_status=-1;
				}
				if(switch_error[1]==1)
				{
					printf("\nERROR 17:  Lines <%d>to <%d> of the switch statement cannot have the case statements with integer labels.",switch_scope_start,switch_scope_end);
			 		semantic_status=-1;
				}
				if(switch_error[2]==1)
				{
					printf("\nERROR 18: The switch statement lines <%d> to <%d> should not have a default statement. ",switch_scope_start,switch_scope_end);
			 		semantic_status=-1;
				}
				switch_error[0]=-1;
				switch_error[1]=-1;
				switch_error[2]=-1;
				SCOPE=(SCOPE->prev);
				free(SCOPE->next);
			}
			else if((SCOPE->in_loop)==1)
			{	
				SCOPE=(SCOPE->prev);
				free(SCOPE->next);
			}
		}
		else if(tok->s==FOR)
		{
			Scope_Chain t = (Scope_Chain) malloc (sizeof (struct scope_chain));
					
			t->scope = ++univ_scope;
			
			t->next=NULL;
			t->prev=SCOPE;
			t->in_switch=0;
			t->in_loop=1;
			SCOPE->next=t;
			SCOPE=(SCOPE->next);
			
			tok->df=SCOPE->scope;			

						
		}
		else if(tok->s==WHILE)
		{
			Scope_Chain t = (Scope_Chain) malloc (sizeof (struct scope_chain));
					
			t->scope = ++univ_scope;
			
			t->next=NULL;
			t->prev=SCOPE;
			t->in_switch=0;
			t->in_loop=1;
			SCOPE->next=t;
			SCOPE=(SCOPE->next);

			tok->df=SCOPE->scope;	
			
		}
		else if(tok->s==START)
		{
			scope_tot[SCOPE->scope].scope_start=tok->n->lno;
		}	
		tok=tok->n;
	}
}
else
{
	
	while(tok!=NULL)
	{
		if(tok->s == MODULE)
		{
			result=find2(h_fn, tok->n->val.s, SCOPE->scope, 1);	
			if(tok->prev->s!=USE)
			{
				if(tok->prev->s==DEF)
				{
					strcpy(current_func,tok->n->val.s);
					if(result.code!=-1)
						if(result.node->entity.ifn.status==declared&&result.node->entity.ifn.used==0)
						{
							//printf("\nERROR 21: Redundant declaration of module \"%s\" ",current_func);
			 				semantic_status=-1;
						}
				}	
				if(result.code==-1)			
				{
					tok=tok->n;			
					IDEnt temp = (IDEnt) malloc (sizeof(struct idEnt));
					strcpy(temp->entity.ifn.word,tok->val.s);
					if(tok->prev->prev->s==DECLARE)
						temp->entity.ifn.status=declared;
					else
					{
						temp->entity.ifn.scope=SCOPE->scope;
						temp->entity.ifn.lno=tok->lno;
						temp->entity.ifn.status=defined;
					}
					addKeyword2(h_fn,temp,1);
				}
				else
				{
					if(tok->prev->s==DEF)
					{	tok=tok->n;
						if(result.node->entity.ifn.status==declared)
						{
							result.node->entity.ifn.scope=SCOPE->scope;
							result.node->entity.ifn.lno=tok->lno;
							result.node->entity.ifn.status=defined;
						}
						else
						{
						//printf("\nERROR 19: Overloading of module \"%s\" at line number %d not allowed",current_func,tok->lno);
			 			semantic_status=-1;
						}
					}
					else if(tok->prev->s==DECLARE)
					{
						tok=tok->n;
						FILE *fp;
						//printf("\nERROR 21: Redundant declaration of module \"%s\" at line number %d",current_func,tok->lno);
			 			semantic_status=-1;
					}
				}
			}
			else
			{
				tok=tok->n;
				if(strcmp(current_func,tok->val.s)==0)
				{
					//printf("\nERROR 23: Function \"%s\" at line number %d cannot be invoked recursively",current_func,tok->lno);
			 		semantic_status=-1;
				}			
				else if(result.code==-1)				
				{	
					//printf("\nERROR 20: Undeclared module \"%s\" at line number %d",tok->val.s,tok->lno);
		 			semantic_status=-1;
				}
				else
				{
					result.node->entity.ifn.used=1;
				}		
			}
		}		
		else if(tok->s == INPUT)
		{
			tok=tok->n->n;
			while (tok->s!=SQBC)
			{
				if(tok->s==COMMA)
					tok=tok->n;
				IDEnt temp = (IDEnt) malloc (sizeof(struct idEnt));
				strcpy(temp->entity.ivar.word,tok->val.s);
				temp->entity.ivar.code=id_num++;
				temp->entity.ivar.lno=tok->lno;
				tok=tok->n->n;
				if(tok->s==ARRAY)
				{
					tok=tok->n->n;
					temp->entity.ivar.var.a.range_s=tok->val.i;
					tok=tok->n->n;
					temp->entity.ivar.var.a.range_f=tok->val.i;
					tok=tok->n->n->n;
					temp->entity.ivar.var.a.type=tok->s;
					temp->entity.ivar.w=assign_width(tok->s);
					temp->entity.ivar.v_or_a=1;
				}
				else
				{
					temp->entity.ivar.var.v.type=tok->s;
					temp->entity.ivar.w=assign_width(tok->s);
					temp->entity.ivar.v_or_a=0;
				}
				temp->entity.ivar.scope=SCOPE->scope;
				result=find2(h_ID,temp->entity.ivar.word,SCOPE->scope,0);
				if(result.code>=0)
				{
					//printf("\nERROR 8: Repeat declaration of identifier \"%s\" at line number %d",temp->entity.ivar.word,temp->entity.ivar.lno);
			 		semantic_status=-1;
				}
				else
				{	
					addKeyword2(h_ID,temp,0);
					result=find2(h_fn,current_func,SCOPE->scope,1);
					if(result.node->entity.ifn.i_start==NULL)
					{
						Parameter temp_par= (Parameter) malloc (sizeof (struct parameter));
						if(temp->entity.ivar.v_or_a==0)
						{
							temp_par->v_or_a=0;
							temp_par->var.v.type=temp->entity.ivar.var.v.type;
							temp_par->next=NULL;
						}
						else
						{
							temp_par->v_or_a=1;
							temp_par->var.a.type=temp->entity.ivar.var.a.type;
							temp_par->var.a.range_s=temp->entity.ivar.var.a.range_s;
							temp_par->var.a.range_f=temp->entity.ivar.var.a.range_f;
							temp_par->next=NULL;
						}
						result.node->entity.ifn.i_start=temp_par;
					}
					else
					{
						Parameter temp_par= result.node->entity.ifn.i_start;
						while(temp_par->next!=NULL)
							temp_par=temp_par->next;
						temp_par->next=(Parameter) malloc (sizeof (struct parameter));
						if(temp->entity.ivar.v_or_a==0)
						{
							temp_par->next->v_or_a=0;
							temp_par->next->var.v.type=temp->entity.ivar.var.v.type;
							temp_par->next->next=NULL;
						}
						else
						{
							temp_par->next->v_or_a=1;
							temp_par->next->var.a.type=temp->entity.ivar.var.a.type;
							temp_par->next->var.a.range_s=temp->entity.ivar.var.a.range_s;
							temp_par->next->var.a.range_f=temp->entity.ivar.var.a.range_f;
							temp_par->next->next=NULL;
						}
					}	
				}
				tok=tok->n;
			}
		
		}
		else if(tok->s == RETURNS)
		{
			tok=tok->n->n;
			while (tok->s!=SQBC)
			{
				if(tok->s==COMMA)
					tok=tok->n;
				IDEnt temp = (IDEnt) malloc (sizeof(struct idEnt));
				strcpy(temp->entity.ivar.word,tok->val.s);
				temp->entity.ivar.code=id_num++;
				temp->entity.ivar.lno=tok->lno;
				tok=tok->n->n;
				temp->entity.ivar.var.v.type=tok->s;
				temp->entity.ivar.w=assign_width(tok->s);
				temp->entity.ivar.v_or_a=0;
				temp->entity.ivar.scope=SCOPE->scope;
				result=find2(h_ID,temp->entity.ivar.word,SCOPE->scope,0);
				if(result.code>=0)
				{
					//printf("\nERROR 8: Repeat declaration of identifier \"%s\" at line number %d",temp->entity.ivar.word,temp->entity.ivar.lno);
			 		semantic_status=-1;
				}
				else
				{	
					addKeyword2(h_ID,temp,0);
					int invalid=check_module_return(SCOPE->scope,tok,h_ID,temp);
					if(invalid==1)
					{					
						//printf("\nERROR 12: The variable \"%s\" returned by the function \"%s\" is not assigned any value within the function definition",temp->entity.ivar.word,current_func);
						semantic_status=-1;
					}
					result=find2(h_fn,current_func,SCOPE->scope,1);
					if(result.node->entity.ifn.o_start==NULL)
					{
						Parameter temp_par= (Parameter) malloc (sizeof (struct parameter));
						if(temp->entity.ivar.v_or_a==0)
						{
							temp_par->v_or_a=0;
							temp_par->var.v.type=temp->entity.ivar.var.v.type;
							temp_par->next=NULL;
						}
						else
						{
							temp_par->v_or_a=1;
							temp_par->var.a.type=temp->entity.ivar.var.a.type;
							temp_par->var.a.range_s=temp->entity.ivar.var.a.range_s;
							temp_par->var.a.range_f=temp->entity.ivar.var.a.range_f;
							temp_par->next=NULL;
						}
						result.node->entity.ifn.o_start=temp_par;
					}
					else
					{
						Parameter temp_par= result.node->entity.ifn.o_start;
						while(temp_par->next!=NULL)
							temp_par=temp_par->next;
						temp_par->next=(Parameter) malloc (sizeof (struct parameter));
						if(temp->entity.ivar.v_or_a==0)
						{
							temp_par->next->v_or_a=0;
							temp_par->next->var.v.type=temp->entity.ivar.var.v.type;
							temp_par->next->next=NULL;
						}
						else
						{
							temp_par->next->v_or_a=1;
							temp_par->next->var.a.type=temp->entity.ivar.var.a.type;
							temp_par->next->var.a.range_s=temp->entity.ivar.var.a.range_s;
							temp_par->next->var.a.range_f=temp->entity.ivar.var.a.range_f;
							temp_par->next->next=NULL;
						}
					}
				}
				tok=tok->n;
			}
		}
		else if(tok->s == DECLARE)
		{
			Symbol type; int V_or_A;int range_s;int range_f;
			if(tok->n->s!=MODULE)
			{
				tok=tok->n;
				IDEnt temp = (IDEnt) malloc (sizeof(struct idEnt));
				strcpy(temp->entity.ivar.word,tok->val.s);
				temp->entity.ivar.code=id_num++;
				temp->entity.ivar.lno=tok->lno;
					while(tok->s!=COLON)
						{tok=tok->n;i++;}
					tok=tok->n;
					if(tok->s==ARRAY)
					{
						tok=tok->n->n;
						temp->entity.ivar.var.a.range_s=tok->val.i;
						range_s=tok->val.i;
						tok=tok->n->n;
						temp->entity.ivar.var.a.range_f=tok->val.i;
						range_f=tok->val.i;
						tok=tok->n->n->n;
						temp->entity.ivar.var.a.type=tok->s;
						type=tok->s;
						temp->entity.ivar.w=assign_width(type);
						temp->entity.ivar.v_or_a=1;
						V_or_A=1;
					}
					else
					{
						temp->entity.ivar.var.v.type=tok->s;
						type=tok->s;
						temp->entity.ivar.w=assign_width(type);
						temp->entity.ivar.v_or_a=0;
						V_or_A=0;
					}
					tok=tok->prev;
					while(i>0)
						{tok=tok->prev;i--;}
					temp->entity.ivar.scope=SCOPE->scope;
					result=find2(h_ID,temp->entity.ivar.word,SCOPE->scope,0);
					if(result.code>=0)
					{
						//printf("\nERROR 8: Repeat declaration of identifier \"%s\" at line number %d",temp->entity.ivar.word,temp->entity.ivar.lno);
			 			semantic_status=-1;
					}
					else
						{addKeyword2(h_ID,temp,0);}		
					if(i==0)
					{while(tok->s!=COLON)
					{
						if(tok->s!=COMMA)
						{	
							IDEnt temp = (IDEnt) malloc (sizeof(struct idEnt));					
							strcpy(temp->entity.ivar.word,tok->val.s);
							if(V_or_A==1)
							{
								temp->entity.ivar.var.a.range_s=range_s;
								temp->entity.ivar.var.a.range_f=range_f;
								temp->entity.ivar.var.a.type=type;
								temp->entity.ivar.w=assign_width(type);
								temp->entity.ivar.v_or_a=1;
							}
							else
							{
								temp->entity.ivar.var.v.type=type;
								temp->entity.ivar.w=assign_width(type);
								temp->entity.ivar.v_or_a=0;
							}		
							temp->entity.ivar.code=id_num++;
							temp->entity.ivar.scope=SCOPE->scope;
							temp->entity.ivar.lno=tok->lno;
							result=find2(h_ID,temp->entity.ivar.word,SCOPE->scope,0);
							if(result.code>=0)
							{
								//printf("\nERROR 8: Repeat declaration of identifier \"%s\" at line number %d",temp->entity.ivar.word,temp->entity.ivar.lno);
			 					semantic_status=-1;
							}
							else
							{addKeyword2(h_ID,temp,0);}
						}
						tok=tok->n;
					}
					}
			}
		i=-2;
		}
		else if(tok->s==ID)
		{
			t = SCOPE;
			while(1)
			{	
				result=find2(h_ID,tok->val.s,t->scope,0);
				if(result.code!=-1)
					break;				
				else
				{
					if(t->prev!=NULL)
						t=(t->prev);
					else
						break;
				}
			}
			
			if(result.code==-1)
			{
				//printf("\nERROR 7: Undeclared variable \"%s\" at line number %d",tok->val.s,tok->lno);
			 	semantic_status=-1;
			}	
		}
		else if(tok->s==DEF)
		{
			SCOPE->scope=++univ_scope;
		}
		else if(tok->s==DRIVERDEF)
		{
			SCOPE->scope=++univ_scope;
			IDEnt temp = (IDEnt) malloc (sizeof(struct idEnt));
			strcpy(temp->entity.ifn.word,"program");
			temp->entity.ifn.status=defined;
			temp->entity.ifn.scope=SCOPE->scope;
			temp->entity.ifn.lno=tok->lno;
			addKeyword2(h_fn,temp,1);			
			strcpy(current_func,"program");
		}
		else if(tok->s==SWITCH)
		{
			switch_scope_start=(tok->lno)+1;
					
			Scope_Chain t = (Scope_Chain) malloc (sizeof (struct scope_chain));
					
			t->scope = ++univ_scope;
			
			t->next=NULL;
			t->prev=SCOPE;
			t->in_switch=0;
			t->in_loop=0;
			SCOPE->next=t;
			SCOPE=(SCOPE->next);

			tok->df=SCOPE->scope;	
			tok=tok->n->n;
			t = SCOPE->prev;
			
			while(1)
			{	
				result=find2(h_ID,tok->val.s,t->scope,0);
				if(result.code!=-1)
					break;				
				else
				{
					if(t->prev!=NULL)
						t=(t->prev);
					else
						break;
				}
			}
			if(result.code==-1)
			{
				//printf("\nERROR 7: Undeclared variable \"%s\" at line number %d",tok->val.s,tok->lno);
			 	semantic_status=-1;
			}
			else
			{
				switch_type=result.node->entity.ivar.var.v.type;
				if(switch_type==REAL)
				{
					//printf("\nERROR 16: Line %d has a switch statement with an identifier of type real ",tok->lno);
			 		semantic_status=-1;
				}
				SCOPE->in_switch=1;
			}
		}
		else if(tok->s==CASE&&SCOPE->in_switch==1)
		{
			tok=tok->n;
			int case_type= tok->s;
			if(case_type!=NUM&&switch_type==INTEGER)
			{
				switch_error[0]=1;
			}
			else if(switch_type==BOOLEAN&&(!(case_type==TRUE_||case_type==FALSE_)))
			{
				switch_error[1]=1;
			}
		}
		else if(tok->s==DEFAULT&&SCOPE->in_switch==1)
		{
			if(switch_type==BOOLEAN)
			{			
				switch_error[2]=1;
			}
			default_flag=1;
		}
		else if(tok->s==END)
		{	
			scope_tot[SCOPE->scope].scope_end=tok->prev->lno;
			tok->df=SCOPE->scope;
			if((SCOPE->in_switch)==1)
			{					
				switch_scope_end = tok->lno;	
				if(default_flag==0&&switch_type!=BOOLEAN)
				{
					//printf("\nERROR 15: The case statements lines <%d> to <%d> must have a default statement.",switch_scope_start,switch_scope_end);
			 		semantic_status=-1;
				}
				default_flag=0;
				if(switch_error[0]==1)
				{
					//printf("\nERROR 14:  The switch statement lines <%d>  to <%d>  cannot have case statement with case keyword followed by any other value other than  an integer ",switch_scope_start,switch_scope_end);
			 		semantic_status=-1;
				}
				if(switch_error[1]==1)
				{
					//printf("\nERROR 17:  Lines <%d>to <%d> of the switch statement cannot have the case statements with integer labels.",switch_scope_start,switch_scope_end);
			 		semantic_status=-1;
				}
				if(switch_error[2]==1)
				{
					//printf("\nERROR 18: The switch statement lines <%d> to <%d> should not have a default statement. ",switch_scope_start,switch_scope_end);
			 		semantic_status=-1;
				}
				switch_error[0]=-1;
				switch_error[1]=-1;
				switch_error[2]=-1;
				SCOPE=(SCOPE->prev);
				free(SCOPE->next);
			}
			else if((SCOPE->in_loop)==1)
			{	
				SCOPE=(SCOPE->prev);
				free(SCOPE->next);
			}
		}
		else if(tok->s==FOR)
		{
			Scope_Chain t = (Scope_Chain) malloc (sizeof (struct scope_chain));
					
			t->scope = ++univ_scope;
			
			t->next=NULL;
			t->prev=SCOPE;
			t->in_switch=0;
			t->in_loop=1;
			SCOPE->next=t;
			SCOPE=(SCOPE->next);
			
			tok->df=SCOPE->scope;			

						
		}
		else if(tok->s==WHILE)
		{
			Scope_Chain t = (Scope_Chain) malloc (sizeof (struct scope_chain));
					
			t->scope = ++univ_scope;
			
			t->next=NULL;
			t->prev=SCOPE;
			t->in_switch=0;
			t->in_loop=1;
			SCOPE->next=t;
			SCOPE=(SCOPE->next);

			tok->df=SCOPE->scope;	
			
		}
		else if(tok->s==START)
		{
			scope_tot[SCOPE->scope].scope_start=tok->n->lno;
		}	
		tok=tok->n;
	}
}
}


void second_run(tokenInfo tok,hashTable2 h_fn, hashTable2 h_ID,int print)
{	
	findresult2 result;
	Scope_Chain t = (Scope_Chain) malloc (sizeof (struct scope_chain));
	Scope_Chain SCOPE = (Scope_Chain) malloc (sizeof (struct scope_chain));;
	SCOPE->scope=0;
	SCOPE->next=NULL;
	SCOPE->prev=NULL;
	SCOPE->in_switch=0;
	SCOPE->in_loop=0;
	int univ_scope=0;
	char current_func[10]; 
	strcpy(current_func," ");
	
	
if(print==1)
{
	while(tok!=NULL)
	{
		if(tok->s == MODULE)
		{
			if(tok->prev->s==DEF)
			{
				strcpy(current_func,tok->n->val.s);
			}
		}
		else if(tok->s==USE)
		{
			int flag;
			tok=tok->n->n;
			result=find2(h_fn, tok->val.s, 0, 1);
			if(strcmp(current_func,tok->val.s)==0)
			{
				while(tok->s!=SEMICOL)
					tok=tok->n;
			}			
			else if(result.code==-1)				
			{	
				while(tok->s!=SEMICOL)
					tok=tok->n;
			}
			else
			{
				while(tok->prev->s!=SEMICOL)
					tok=tok->prev;
				findresult2 actual_par;
				Parameter formal_par = result.node->entity.ifn.o_start;			
				if(tok->s!=SQBO&&formal_par!=NULL)
				{
					printf("\nThe values returned by function \"%s\" are not stored at line number %d",result.node->entity.ifn.word,tok->lno);
			 		semantic_status=-1;
				}
				else if(tok->s==SQBO&&formal_par==NULL)
				{
					printf("\nERROR 13: The function \"%s\" should  not return any value at line number %d",result.node->entity.ifn.word,tok->lno);
			 		semantic_status=-1;
				}
				else if(tok->s==SQBO&&formal_par!=NULL)
				{
					tok=tok->n;
					t = SCOPE;
					while(1)
					{	
						actual_par=find2(h_ID,tok->val.s,t->scope,0);
						if(actual_par.code!=-1)
							break;				
						else
						{
							if(t->prev!=NULL)
								t=(t->prev);
							else
							break;
						}
					}
					flag=0;
					while(formal_par!=NULL)
					{
				  		if(actual_par.code!=-1)
				  		{	
							if(actual_par.node->entity.ivar.var.v.type!=formal_par->var.v.type)
							{
								printf("\nERROR 9: Type mismatch of output parameter \"%s\" at line number %d ",actual_par.node->entity.ivar.word,tok->lno);
		 						semantic_status=-1;
							}
 						}
				    		formal_par=formal_par->next;
						tok=tok->n->n;
						if(tok->prev->s!=SQBC)
						{
							t = SCOPE;
							while(1)
							{	
								actual_par=find2(h_ID,tok->val.s,t->scope,0);
								if(actual_par.code!=-1)
									break;				
								else
								{
									if(t->prev!=NULL)
										t=(t->prev);
									else
										break;
								}
							}
						}
						else
						{	
							tok=tok->prev;
							flag=1;
							break;
						}
					}
					if(flag==1&&formal_par!=NULL)
					{
						printf("\nThe number of values returned from function \"%s\" are less than the number of accepting at line number %d",result.node->entity.ifn.word,tok->lno);
			 			semantic_status=-1;
					}
					else if(flag==0)
					{
						printf("\nThe number of values returned from function \"%s\" are more than the number of accepting at line number %d",result.node->entity.ifn.word,tok->lno);
			 			semantic_status=-1;
					}
				}
				flag=0;
				while(tok->s!=USE)
					tok=tok->n;
				tok=tok->n->n;
			
				tok=tok->n->n->n;
				formal_par = result.node->entity.ifn.i_start;
				t = SCOPE;
					while(1)
					{	
						actual_par=find2(h_ID,tok->val.s,t->scope,0);
						if(actual_par.code!=-1)
							break;				
						else
						{
							if(t->prev!=NULL)
								t=(t->prev);
							else
							break;
						}
					}
				int flag=0;
				while(formal_par!=NULL)
				{
				    if(actual_par.code!=-1)
				    {	if(actual_par.node->entity.ivar.v_or_a==0)
					{
						if(formal_par->v_or_a!=0)
						{
							printf("\nParameter \"%s\" at line number %d is an array, whereas a variable type was  expected",actual_par.node->entity.ivar.word,tok->lno);
			 				semantic_status=-1;
						}
						else
						{
							if(actual_par.node->entity.ivar.var.v.type!=formal_par->var.v.type)
							{
								printf("\nERROR 10a: Type mismatch of input parameter \"%s\" at line number %d ",actual_par.node->entity.ivar.word,tok->lno);
			 					semantic_status=-1;
							}
						}	
					}
					else
					{
						if(formal_par->v_or_a!=1)
						{
							printf("\nParameter \"%s\" at line number %d is a variable type, whereas array type was  expected",actual_par.node->entity.ivar.word,tok->lno);
			 				semantic_status=-1;
						}
						else
						{
							if(actual_par.node->entity.ivar.var.a.type!=formal_par->var.a.type)
							{
								printf("\nERROR 10a: Type mismatch of input parameter \"%s\" at line number %d ",actual_par.node->entity.ivar.word,tok->lno);
			 					semantic_status=-1;
							}
							if((actual_par.node->entity.ivar.var.a.range_s!=formal_par->var.a.range_s)&&(actual_par.node->entity.ivar.var.a.range_f!=formal_par->var.a.range_f))
							{
								printf("\nERROR 10b: Range mismatch of input parameter \"%s\" at line number %d ",actual_par.node->entity.ivar.word,tok->lno);
			 					semantic_status=-1;
							}
						
						}
					}
				    }
				    	formal_par=formal_par->next;
					tok=tok->n->n;
					if(tok->prev->s!=SEMICOL)
					{
						t = SCOPE;
						while(1)
						{	
							actual_par=find2(h_ID,tok->val.s,t->scope,0);
							if(actual_par.code!=-1)
								break;				
							else
							{
								if(t->prev!=NULL)
									t=(t->prev);
								else
									break;
							}
						}
					}
					else
					{	
						tok=tok->prev;
						flag=1;
						break;
					}
				}
				if(flag==1&&formal_par!=NULL)
				{
					printf("\nERROR 11a: The number of formal parameters of function \"%s\" are more than that of the actual parameters at line number %d",result.node->entity.ifn.word,tok->lno);
			 		semantic_status=-1;
				}
				else if(flag==0)
				{
					printf("\nERROR 11b: The number of formal parameters of function \"%s\" are less than that of the actual parameters at line number %d",result.node->entity.ifn.word,tok->lno);
			 		semantic_status=-1;					
				}
			}
		}
		else if(tok->s==DEF)
		{
			SCOPE->scope=(++univ_scope);
		}
		else if(tok->s==DRIVERDEF)
		{
			SCOPE->scope=(++univ_scope);
			strcpy(current_func,"program");
		}
		else if(tok->s==SWITCH)
		{
			Scope_Chain t = (Scope_Chain) malloc (sizeof (struct scope_chain));
			t->scope = (++univ_scope);
			t->next=NULL;
			t->prev=SCOPE;
			t->in_switch=0;
			t->in_loop=0;
			SCOPE->next=t;
			SCOPE=(SCOPE->next);
			
			tok=tok->n->n;
			t = SCOPE->prev;
			while(1)
			{	
				result=find2(h_ID,tok->val.s,t->scope,0);
				if(result.code!=-1)
					break;				
				else
				{
					if(t->prev!=NULL)
						t=(t->prev);
					else
						break;
				}
			}
			if(result.code!=-1)
			{
				SCOPE->in_switch=1;
			}
		}
		else if(tok->s==END&&SCOPE->in_switch==1)
		{
			SCOPE=(SCOPE->prev);
			free(SCOPE->next);
		}
		else if(tok->s==FOR)
		{
			Scope_Chain t = (Scope_Chain) malloc (sizeof (struct scope_chain));
					
			t->scope = ++univ_scope;
			
			t->next=NULL;
			t->prev=SCOPE;
			t->in_switch=0;
			t->in_loop=1;
			SCOPE->next=t;
			SCOPE=(SCOPE->next);
			tok=tok->n->n;
			t = SCOPE->prev;
			findresult2 control_var; 
			while(1)
			{	
				control_var=find2(h_ID,tok->val.s,t->scope,0);
				if(control_var.code!=-1)
					break;				
				else
				{
					if(t->prev!=NULL)
						t=(t->prev);
					else
						break;
				}
			}
			check_for(tok->prev->prev,h_ID,control_var.node,print);
		}
		else if(tok->s==WHILE)
		{
			Scope_Chain t = (Scope_Chain) malloc (sizeof (struct scope_chain));
					
			t->scope = ++univ_scope;
			
			t->next=NULL;
			t->prev=SCOPE;
			t->in_switch=0;
			t->in_loop=1;
			SCOPE->next=t;
			SCOPE=(SCOPE->next);
			
		}
		else if(tok->s==END&&(SCOPE->in_loop)==1)
		{
			SCOPE=(SCOPE->prev);
			free(SCOPE->next);
		}
		
		
		tok=tok->n;
	}
}
else
{
	while(tok!=NULL)
	{
		if(tok->s == MODULE)
		{
			if(tok->prev->s==DEF)
			{
				strcpy(current_func,tok->n->val.s);
			}
		}
		else if(tok->s==USE)
		{
			int flag;
			tok=tok->n->n;
			result=find2(h_fn, tok->val.s, 0, 1);
			if(strcmp(current_func,tok->val.s)==0)
			{
				while(tok->s!=SEMICOL)
					tok=tok->n;
			}			
			else if(result.code==-1)				
			{	
				while(tok->s!=SEMICOL)
					tok=tok->n;
			}
			else
			{
				while(tok->prev->s!=SEMICOL)
					tok=tok->prev;
				findresult2 actual_par;
				Parameter formal_par = result.node->entity.ifn.o_start;			
				if(tok->s!=SQBO&&formal_par!=NULL)
				{
					//printf("\nThe values returned by function \"%s\" are not stored at line number %d",result.node->entity.ifn.word,tok->lno);
			 		semantic_status=-1;
				}
				else if(tok->s==SQBO&&formal_par==NULL)
				{
					//printf("\nERROR 13: The function \"%s\" should  not return any value at line number %d",result.node->entity.ifn.word,tok->lno);
			 		semantic_status=-1;
				}
				else if(tok->s==SQBO&&formal_par!=NULL)
				{
					tok=tok->n;
					t = SCOPE;
					while(1)
					{	
						actual_par=find2(h_ID,tok->val.s,t->scope,0);
						if(actual_par.code!=-1)
							break;				
						else
						{
							if(t->prev!=NULL)
								t=(t->prev);
							else
							break;
						}
					}
					flag=0;
					while(formal_par!=NULL)
					{
				  		if(actual_par.code!=-1)
				  		{	
							if(actual_par.node->entity.ivar.var.v.type!=formal_par->var.v.type)
							{
								//printf("\nERROR 9: Type mismatch of output parameter \"%s\" at line number %d ",actual_par.node->entity.ivar.word,tok->lno);
		 						semantic_status=-1;
							}
 						}
				    		formal_par=formal_par->next;
						tok=tok->n->n;
						if(tok->prev->s!=SQBC)
						{
							t = SCOPE;
							while(1)
							{	
								actual_par=find2(h_ID,tok->val.s,t->scope,0);
								if(actual_par.code!=-1)
									break;				
								else
								{
									if(t->prev!=NULL)
										t=(t->prev);
									else
										break;
								}
							}
						}
						else
						{	
							tok=tok->prev;
							flag=1;
							break;
						}
					}
					if(flag==1&&formal_par!=NULL)
					{
						//printf("\nThe number of values returned from function \"%s\" are less than the number of accepting at line number %d",result.node->entity.ifn.word,tok->lno);
			 			semantic_status=-1;
					}
					else if(flag==0)
					{
						//printf("\nThe number of values returned from function \"%s\" are more than the number of accepting at line number %d",result.node->entity.ifn.word,tok->lno);
			 			semantic_status=-1;
					}
				}
				flag=0;
				while(tok->s!=USE)
					tok=tok->n;
				tok=tok->n->n;
			
				tok=tok->n->n->n;
				formal_par = result.node->entity.ifn.i_start;
				t = SCOPE;
					while(1)
					{	
						actual_par=find2(h_ID,tok->val.s,t->scope,0);
						if(actual_par.code!=-1)
							break;				
						else
						{
							if(t->prev!=NULL)
								t=(t->prev);
							else
							break;
						}
					}
				int flag=0;
				while(formal_par!=NULL)
				{
				    if(actual_par.code!=-1)
				    {	if(actual_par.node->entity.ivar.v_or_a==0)
					{
						if(formal_par->v_or_a!=0)
						{
							//printf("\nParameter \"%s\" at line number %d is an array, whereas a variable type was  expected",actual_par.node->entity.ivar.word,tok->lno);
			 				semantic_status=-1;
						}
						else
						{
							if(actual_par.node->entity.ivar.var.v.type!=formal_par->var.v.type)
							{
								//printf("\nERROR 10a: Type mismatch of input parameter \"%s\" at line number %d ",actual_par.node->entity.ivar.word,tok->lno);
			 					semantic_status=-1;
							}
						}	
					}
					else
					{
						if(formal_par->v_or_a!=1)
						{
							//printf("\nParameter \"%s\" at line number %d is a variable type, whereas array type was  expected",actual_par.node->entity.ivar.word,tok->lno);
			 				semantic_status=-1;
						}
						else
						{
							if(actual_par.node->entity.ivar.var.a.type!=formal_par->var.a.type)
							{
								//printf("\nERROR 10a: Type mismatch of input parameter \"%s\" at line number %d ",actual_par.node->entity.ivar.word,tok->lno);
			 					semantic_status=-1;
							}
							if((actual_par.node->entity.ivar.var.a.range_s!=formal_par->var.a.range_s)&&(actual_par.node->entity.ivar.var.a.range_f!=formal_par->var.a.range_f))
							{
								//printf("\nERROR 10b: Range mismatch of input parameter \"%s\" at line number %d ",actual_par.node->entity.ivar.word,tok->lno);
			 					semantic_status=-1;
							}
						
						}
					}
				    }
				    	formal_par=formal_par->next;
					tok=tok->n->n;
					if(tok->prev->s!=SEMICOL)
					{
						t = SCOPE;
						while(1)
						{	
							actual_par=find2(h_ID,tok->val.s,t->scope,0);
							if(actual_par.code!=-1)
								break;				
							else
							{
								if(t->prev!=NULL)
									t=(t->prev);
								else
									break;
							}
						}
					}
					else
					{	
						tok=tok->prev;
						flag=1;
						break;
					}
				}
				if(flag==1&&formal_par!=NULL)
				{
					//printf("\nERROR 11a: The number of formal parameters of function \"%s\" are more than that of the actual parameters at line number %d",result.node->entity.ifn.word,tok->lno);
			 		semantic_status=-1;
				}
				else if(flag==0)
				{
					//printf("\nERROR 11b: The number of formal parameters of function \"%s\" are less than that of the actual parameters at line number %d",result.node->entity.ifn.word,tok->lno);
			 		semantic_status=-1;					
				}
			}
		}
		else if(tok->s==DEF)
		{
			SCOPE->scope=(++univ_scope);
		}
		else if(tok->s==DRIVERDEF)
		{
			SCOPE->scope=(++univ_scope);
			strcpy(current_func,"program");
		}
		else if(tok->s==SWITCH)
		{
			Scope_Chain t = (Scope_Chain) malloc (sizeof (struct scope_chain));
			t->scope = (++univ_scope);
			t->next=NULL;
			t->prev=SCOPE;
			t->in_switch=0;
			t->in_loop=0;
			SCOPE->next=t;
			SCOPE=(SCOPE->next);
			
			tok=tok->n->n;
			t = SCOPE->prev;
			while(1)
			{	
				result=find2(h_ID,tok->val.s,t->scope,0);
				if(result.code!=-1)
					break;				
				else
				{
					if(t->prev!=NULL)
						t=(t->prev);
					else
						break;
				}
			}
			if(result.code!=-1)
			{
				SCOPE->in_switch=1;
			}
		}
		else if(tok->s==END&&SCOPE->in_switch==1)
		{
			SCOPE=(SCOPE->prev);
			free(SCOPE->next);
		}
		else if(tok->s==FOR)
		{
			Scope_Chain t = (Scope_Chain) malloc (sizeof (struct scope_chain));
					
			t->scope = ++univ_scope;
			
			t->next=NULL;
			t->prev=SCOPE;
			t->in_switch=0;
			t->in_loop=1;
			SCOPE->next=t;
			SCOPE=(SCOPE->next);
			tok=tok->n->n;
			t = SCOPE->prev;
			findresult2 control_var; 
			while(1)
			{	
				control_var=find2(h_ID,tok->val.s,t->scope,0);
				if(control_var.code!=-1)
					break;				
				else
				{
					if(t->prev!=NULL)
						t=(t->prev);
					else
						break;
				}
			}
			check_for(tok->prev->prev,h_ID,control_var.node,print);
		}
		else if(tok->s==WHILE)
		{
			Scope_Chain t = (Scope_Chain) malloc (sizeof (struct scope_chain));
					
			t->scope = ++univ_scope;
			
			t->next=NULL;
			t->prev=SCOPE;
			t->in_switch=0;
			t->in_loop=1;
			SCOPE->next=t;
			SCOPE=(SCOPE->next);
			
		}
		else if(tok->s==END&&(SCOPE->in_loop)==1)
		{
			SCOPE=(SCOPE->prev);
			free(SCOPE->next);
		}
		
		
		tok=tok->n;
	}
}

}

int check_module_return(int end_point,tokenInfo t, hashTable2 h_ID, IDEnt control_var)
{
	int flag=0;
	while(t->s!=START)
		t=t->n;
	t=t->n;
		while(!(t->s==DEF||t->s==DRIVERDEF))
		{
			if(t->s!=ID)
			{
				if(t->n!=NULL)	
				{	t=t->n;
					continue;
				}
				else
					break;
			}
			if(strcmp(control_var->entity.ivar.word,t->val.s)!=0)
			{
				if(t->n!=NULL)	
				{	t=t->n;
					continue;
				}
				else
					break;
			}
			if(t->prev->prev->s==GET_VALUE)
			{		
				flag=1;
				break;
			}
			t=t->n;
			while(t->s!=SEMICOL)
			{
				if(t->s==ASSIGNOP)
				{
					flag=1;
					break;
				}
				t=t->n;
			}
			if(flag==1)
				break;
		}
		if(flag==0)
			return 1;
		else
			return 0;
	
}	

void check_for(tokenInfo t, hashTable2 h_ID, IDEnt control_var, int print)
	{	
		int end_point=t->df;
		int for_flag;
		while(t->s!=START)
			t=t->n;
		t=t->n;
		while(!(t->df==end_point&&t->s==END))
		{
			for_flag=0;
			if(t->s!=ID)
			{
				t=t->n;
				continue;
			}
			if(strcmp(control_var->entity.ivar.word,t->val.s)!=0)
			{
				t=t->n;
				continue;
			}
			if(t->prev->prev->s==GET_VALUE)
			{
				if(print==1)printf("\nERROR 22: The for statement must not redefine the variable \"%s\" at line number %d",control_var->entity.ifn.word,t->lno);
				semantic_status=-1;
				t=t->n;
				continue;
			}
			tokenInfo temp=t;
			while(temp->prev->s!=SEMICOL)
				temp=temp->prev;
			if(temp->s==ID&&temp->n->s==SQBO)
				for_flag=1;
			t=t->n;
			
			while(t->s!=SEMICOL)
			{
				if(t->s==ASSIGNOP&&for_flag==0)
				{
					if(print==1)printf("\nERROR 22: The for statement must not redefine the variable \"%s\" at line number %d",control_var->entity.ifn.word,t->lno);
			 		semantic_status=-1;
					while(t->s!=SEMICOL)
						t=t->n;
					break;
				}
				t=t->n;
			}
		}
	}
		
void print_ST(FILE *fp, hashTable2 h, Total_Scope_List t, int option)
{	
	int index=0;
	if(option==0)
	{
		while (index < 13)
		{
			chain2 temp = h[index].start;
			if(h[index].flag==occupied)		
			{	
				if(h[index].ID->entity.ivar.v_or_a==0)	
					fprintf(fp,"%s\t\t %d\t\t %d\t\t %s\t\t %d\n",h[index].ID->entity.ivar.word,t[h[index].ID->entity.ivar.scope].scope_start,t[h[index].ID->entity.ivar.scope].scope_end, type_get(h[index].ID->entity.ivar.var.v.type),h[index].ID->entity.ivar.w);
				else
					fprintf(fp,"%s\t\t %d\t\t %d\t\t ARRAY:%s\t\t %d\n",h[index].ID->entity.ivar.word,t[h[index].ID->entity.ivar.scope].scope_start,t[h[index].ID->entity.ivar.scope].scope_end, type_get(h[index].ID->entity.ivar.var.a.type),((h[index].ID->entity.ivar.var.a.range_f-h[index].ID->entity.ivar.var.a.range_s+1)*h[index].ID->entity.ivar.w));
				while(temp!=NULL)
				{
					if(temp->ID->entity.ivar.v_or_a==0)
						fprintf(fp,"%s\t\t %d\t\t %d\t\t %s\t\t %d\n",temp->ID->entity.ivar.word,t[temp->ID->entity.ivar.scope].scope_start,t[temp->ID->entity.ivar.scope].scope_end, type_get(temp->ID->entity.ivar.var.v.type),temp->ID->entity.ivar.w);
					else
						fprintf(fp,"%s\t\t %d\t\t %d\t\t ARRAY:%s\t\t %d\n",temp->ID->entity.ivar.word,t[temp->ID->entity.ivar.scope].scope_start,t[temp->ID->entity.ivar.scope].scope_end, type_get(temp->ID->entity.ivar.var.a.type),((temp->ID->entity.ivar.var.a.range_f-temp->ID->entity.ivar.var.a.range_s+1)*temp->ID->entity.ivar.w));
					temp=temp->next;
				}
			}	
		index ++;
		}
	}
	else
	{
		while (index < 13)
		{
			chain2 temp = h[index].start;
			if(h[index].flag==occupied)		
			{		
				fprintf(fp,"%s\t\t %d\t\t %d\n",h[index].ID->entity.ifn.word,t[h[index].ID->entity.ifn.scope].scope_start,t[h[index].ID->entity.ifn.scope].scope_end);
				while(temp!=NULL)
				{
					fprintf(fp,"%s\t\t %d\t\t %d\n",temp->ID->entity.ifn.word,t[temp->ID->entity.ifn.scope].scope_start,t[temp->ID->entity.ifn.scope].scope_end);
					temp=temp->next;
				}
			}	
		index ++;
		}
	}
	fprintf(fp,"\n");
		
}

char* type_get(Symbol s)
{
	if(s==INTEGER)
		return "INTEGER";
	else if(s==REAL)
		return "REAL";
	else
		return "BOOLEAN";
}

int assign_width(Symbol s)
{
	if(s==INTEGER)
		return 2;
	else if(s==REAL)
		return 4;
	else
		return 1;
}
			
