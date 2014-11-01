#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

static table T;

parseTree parseInputSourceCode(char * testCaseFile, hashTable h, Grammar G, int print)
{

	int i;int flag=0;
	parseTree P;
	P.root=NULL;
	P.height=0;
	P.currentNode=P.root;

	FILE *fp;
	fp= fopen(testCaseFile, "r");

	// populating Symbol Table and creating the Token file
		
	populateSymbolTable(testCaseFile,h,print);

	// Initializing stack to caontain the $, start Symbol= program;

	parseTreeStack stack;
	stack.top=createGrammarNode((G_element)(terminal)$,term);
	stack.size++;


	gnode temporary;
	temporary=createGrammarNode((G_element)(nonTerminal)program,non_term);
	temporary->next=stack.top;
	stack.top=temporary;
	stack.size++;


	//Program to create the Parse Tree
	char * stackToken="\0";char *token="\0";char * lexeme="\0";
	int rule,stacksize_i,stacksize_f;
	pnode temp;
	tokenInfo currentToken, nullToken;
	nullToken=NULL;
	currentToken=getNextToken();

	while( (((stack.top->t==term)&&(stack.top->ge.t!=$))||(stack.top->t=non_term))&&(currentToken!=NULL))

	{ 
	  G=createGrammar();
	  if (stack.top->t==term)
		 { 
			stackToken=getToken(stack.top->ge.t);
			if (stack.top->ge.t==e)
			{stack=pop(stack);
			  temp=createPnodeTerminal(P.currentNode,(terminal)e,(tag)term,P.currentNode->level+1,nullToken);   
				P.currentNode->child=enqueue(P.currentNode,temp);
			 if (P.currentNode->level==P.height)
			  P.height=P.currentNode->level+1;
			 
				 P.currentNode=returnCurrent(P.currentNode);
				
	 
			 }
		  else 
		  // if term is not e
			 { 
			   if (stack.top->ge.t==currentToken->s) //if stack matches input
				   { stack=pop(stack);
					 temp=createPnodeTerminal(P.currentNode,(terminal)currentToken->s,(tag)term,P.currentNode->level+1,currentToken);
					 P.currentNode->child=enqueue(P.currentNode,temp);
					 if (P.currentNode->level==P.height)
					   P.height=P.currentNode->level+1;
					
						P.currentNode=returnCurrent(P.currentNode);
						 currentToken=getNextToken();
				   }
			   else  //if stack does not match input;
					{token=getToken(currentToken->s);printf("\n%d\n",stack.top->ge.t);
					 stackToken=getToken(stack.top->ge.t);
					 if (currentToken->s==NUM)
							   printf("\nThe token (%s) for lexeme (%d) does not match at line no. %d. The expected token here is %s", token,currentToken->val.i,currentToken->lno,stackToken);
			else if (currentToken->s==RNUM)
								printf("\nThe token (%s) for lexeme (%f) does not match at line no. %d. The expected token here is %s", token,currentToken->val.f,currentToken->lno,stackToken);
			else if (currentToken->s==ID)
						printf("\nThe token (%s) for lexeme (%s) does not match at line no. %d. The expected token here is %s", token,currentToken->val.s,currentToken->lno,stackToken);
				   else 
						 {lexeme=getLexeme(currentToken->s);
						   printf("\nThe token (%s) for lexeme (%s) does not match at line no. %d. The expected token here is %s", token,lexeme,currentToken->lno,stackToken);
						  }
				printf("\n%d\n",stack.top->ge.t);
				printf("\nParsing not successful\n");flag=1;
					  break;
					 }
				}
		 }//end of if terminal
	// if stack.top is non_term
	  else
		 { stackToken=getNON_term(stack.top->ge.nt);
		   rule=T[(int)stack.top->ge.nt][(int)currentToken->s];
		   if (rule!=-1) // Valid rule
			 { stacksize_i=stack.size-1;
			   
			   stack=(parseTreeStack)mergeStack(G[rule],stack);
			   stacksize_f=stack.size;
			   
			   
				if (P.currentNode==NULL)
			  {temp=createPnodeNonTerminal(P.currentNode,(nonTerminal)G[rule].t,(tag)non_term,stacksize_f-stacksize_i,0);
					   P.root=temp;
			   P.currentNode=temp;
			  }
				 else
					 {temp=createPnodeNonTerminal(P.currentNode,(nonTerminal)G[rule].t,(tag)non_term,stacksize_f-stacksize_i,P.currentNode->level+1);
					 
					   P.currentNode->child=enqueue(P.currentNode,temp);
					 
					   if (P.currentNode->level==P.height)
							 P.height=P.currentNode->level+1;
					   P.currentNode=P.currentNode->child.back;
					  }
		 }
		   // invalid rule
		   else
			 { token=getToken(currentToken->s);
			   if (currentToken->s==NUM)
					  printf("\nThe token (%s) for lexeme (%d) does not match at line no. %d. The expected token here are", token,currentToken->val.i,currentToken->lno,stackToken);
			   else if (currentToken->s==RNUM)
					  printf("\nThe token (%s) for lexeme (%f) does not match at line no. %d. The expected token here are", token,currentToken->val.f,currentToken->lno,stackToken);
		   else if (currentToken->s==ID) 
			printf("\nThe token (%s) for lexeme (%s) does not match at line no. %d. The expected token here are", token,currentToken->val.s,currentToken->lno,stackToken);
			   else 
					{     lexeme=getLexeme(currentToken->s);
						   printf("\nThe token (%s) for lexeme (%s) does not match at line no. %d. The expected token here is %s", token,lexeme,currentToken->lno,stackToken);
					}
		for (i=0;i<57;++i)
			  if (T[(int)stack.top->ge.nt][i]!=-1)
				  {stackToken=getToken(i);
						   printf("\n %s",stackToken);
						   }

		printf("\nParsing not successful\n");flag=1;
			break;
			   }  // end of invalid rule;
		   }//end of stack.top=non_terminal;
	  
	}//end of while
	if(flag==0)
	printf("\nParsing successful\n");
	else parse_status=-1;
	return P;
}

pnode createPnodeNonTerminal(pnode Parent,nonTerminal nt,tag t,int no_of_children, int level)
{
	 pnode temp;
	 temp=(pnode)malloc(sizeof(struct P_node));
	 temp->parent=Parent;
	 temp->child=initializeQueue();
	 temp->e.nt=nt;
	 temp->t=t;
	 temp->no_of_children=no_of_children;
	 temp->level=level;
	 temp->tokenLink=NULL;
	 temp->next=NULL;  
	return temp;
}

pnode returnCurrent(pnode p)
{
while (p->child.stacksize==p->no_of_children)
{p=p->parent;
}
return p;
}

pnode createPnodeTerminal(pnode Parent,terminal term_1,tag t,int level,tokenInfo token)
{     
	pnode temp;
	temp=(pnode)malloc(sizeof(struct P_node));
	temp->parent=Parent;
	temp->child=initializeQueue();
	temp->e.t=term_1;
	temp->t=t;
	temp->no_of_children=0;
	temp->level=level;
	temp->tokenLink=token;
	temp->next=NULL;  
	return temp;            
}

parseTreeStack pop(parseTreeStack s)
{
	parseTreeStack s1;
	gnode temp = s.top;
	s1.top = s.top->next;
	s1.size=s.size-1;
	free(temp);
	return(s1);
}

void printgHead(gHead S)
{
	char * s;
	while (S.top!=NULL)
	{
		if (S.top->t==term)
		{  
			s=getToken(S.top->ge.t); 
		}
		else
		{
			s=getNON_term(S.top->ge.t);
		}
		S.top=S.top->next;
	}
}

parseTreeStack mergeStack(gHead g1,parseTreeStack s)
{
	printgHead(g1);
	s=pop(s);
	parseTreeStack s1;
	s1.top=NULL;
	s1.size=0;
	gnode temp;
	while (g1.top!=NULL)
	{ 
		temp=g1.top;
  		g1.top=g1.top->next;
		if (s1.size==0)
		{
			temp->next=NULL;
 		}
		else 
 		{
  			temp->next=s1.top;
 		} 
		s1.top=temp;
		s1.size++;
	}
	while (s1.size!=0)
	{
		temp=s1.top;
		s1.top=s1.top->next;
		s1.size--;
		temp->next=s.top;
		s.top=temp;
		s.size++;
	}
	return s;
}



pTqueue enqueue(pnode q,pnode p)
{
	p->parent=q;
	if (q->child.stacksize==0)
	  {q->child.front=p;
	   q->child.back=q->child.front;
	   }
	else
	   {q->child.back->next=p;
		q->child.back=p;
		if (q->child.front->next==NULL)
		   q->child.front->next==q->child.back;
		}
	q->child.stacksize++;

	return q->child;
}

pTqueue initializeQueue()
{
    pTqueue q;
	q.back=NULL;
	q.stacksize=0;
	q.front=NULL;
	return q;
}


void printParseTreeQueue (FILE *fp,pnode p)
{
	while (p!=NULL)
	{
	printNode(fp,p);
	if (p->child.front!=NULL)
	  printParseTreeQueue(fp,p->child.front);
	p=p->next;
	}
}

void printParseTree(char *outfile,  parseTree Tree)
{
	FILE *  fp;
	fp=fopen(outfile,"w");
	char * s1;char *s2;char *s3;char * s4;char * s5;char * s6;char * s7;
	s1=(char *)"lexCurrentNode";s2=(char *)"lNo";s3=(char *)"token";s4=(char *)"valIfNo";s5=(char *)"pNode";s6=(char *)"IsLeaf(y/n)";s7=(char *)"nodeSymbol";
	fprintf(fp,"\n%-15s:%-5s:%-12s:%-10s:%-20s:%-12s:%-20s",s1,s2,s3,s4,s5,s6,s7);
	printParseTreeQueue(fp,Tree.root);
	fclose(fp);
}

void printNode(FILE *fp, pnode N)
{
	char * p;char * s;
	char * s8;
	s8=(char *)"e";
	char * n="---";
	char * y="y";
	char * no="n"; 
	if (N->no_of_children==0)
	   if (N->e.t!=e)
		  {
		   s=getToken(N->tokenLink->s);
		   p=getNON_term(N->parent->e.nt);
		  if (N->e.t==NUM)
				 fprintf(fp,"\n%-15d:%-5d:%-12s:%-10d:%-20s:%-12s:%-20s",N->tokenLink->val.i,N->tokenLink->lno,s,N->tokenLink->val.i,p,y,s);
		  else 
			  if (N->e.t==RNUM)
				 fprintf(fp,"\n%-15f:%-5d:%-12s:%-10f:%-20s:%-10s:%-20s",N->tokenLink->val.f,N->tokenLink->lno,s,N->tokenLink->val.f,p,y,s);
		   
			  else
					if (N->e.t==ID)
					   fprintf(fp,"\n%-15s:%-5d:%-12s:%-10s:%-20s:%-10s:%-20s",N->tokenLink->val.s,N->tokenLink->lno,s,n,p,y,s); 
		  
					else
					  {char * lex;
					   lex=getLexeme(N->tokenLink->s);
					   fprintf(fp,"\n%-15s:%-5d:%-12s:%-10s:%-20s:%-10s:%-20s",lex,N->tokenLink->lno,s,n,p,y,s);
					  }
		   }

	   else 
		   {
			p=getNON_term(N->parent->e.nt);
			fprintf(fp,"\n%-15s:%-5s:%-12s:%-10s:%-20s:%-10s%-20s",s8,n,n,n,p,y,n);}
	else
	{
	 if (N->parent!=NULL)p=getNON_term(N->parent->e.nt);
	 else p="NULL";
	 s=getNON_term(N->e.nt);
	fprintf(fp,"\n%-15s:%-5s:%-12s:%-10s:%-20s:%-10s:%-20s",n,n,n,n,p,no,s);
	}
}


/////////////////////////////////////////////////////////////

gnode createGrammarNode(G_element ge, tag t)
{
	gnode n1 = (gnode)malloc(sizeof(struct G_node));
	if(n1!=NULL)
	{n1->ge=ge;
	n1->t=t;
	n1->next = NULL;}
	return n1;
}


Grammar createGrammar()
{
	int i=0;
	gnode g;
	Grammar G=(Grammar)malloc(GRAMCONST*sizeof(gHead));

	//-0- <program> -> <moduleDeclarations> <otherModules><driverModule><otherModules>  
	G[i].t=program;
	g = createGrammarNode((G_element)(nonTerminal)otherModules,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)driverModule,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)otherModules,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)moduleDeclarations,non_term);
	g->next=G[i].top;
	G[i].top=g;

	// -1- <moduleDeclarations> -> <moduleDeclaration><moduleDeclarations> 
	i++;
	G[i].t=moduleDeclarations;
	g= createGrammarNode((G_element)(nonTerminal)moduleDeclaration,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)moduleDeclarations,non_term);
	G[i].top->next=g;

	// -2- <moduleDeclarations> -> e
	i++;
	G[i].t=moduleDeclarations;
	g= createGrammarNode((G_element)(terminal)e,term);
	G[i].top=g;

	// -3- <moduleDeclaration> -> DECLARE MODULE ID SEMICOL 
	i++;
	G[i].t=moduleDeclaration;
	g= createGrammarNode((G_element)(terminal)DECLARE,term);
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)MODULE,term);
	G[i].top->next=g;
	g= createGrammarNode((G_element)(terminal)ID,term);
	G[i].top->next->next=g;
	g= createGrammarNode((G_element)(terminal)SEMICOL,term);
	G[i].top->next->next->next=g;

	// -4- <otherModules> -> <module><otherModules>
	i++;
	G[i].t=otherModules;
	g= createGrammarNode((G_element)(nonTerminal)module,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)otherModules,non_term);
	G[i].top->next=g;

	// -5-  <otherModules> -> e
	i++;
	G[i].t=otherModules;
	g= createGrammarNode((G_element)(terminal)e,term);
	G[i].top=g;

	// -6- <driverModule> -> DRIVERDEF DRIVER PROGRAM DRIVERENDDEF <moduleDef> 
	i++;
	G[i].t=driverModule;
	g = createGrammarNode((G_element)(nonTerminal)moduleDef,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)DRIVERENDDEF,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)PROGRAM,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)DRIVER,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)DRIVERDEF,term);
	g->next=G[i].top;
	G[i].top=g;

	// -7- <module> -> DEF MODULE ID ENDDEF TAKES INPUT SQBO <input_plist> SQBC SEMICOL <ret><moduleDef> 
	i++;
	G[i].t=module;
	g = createGrammarNode((G_element)(nonTerminal)moduleDef,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)ret,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)SEMICOL,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)SQBC,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)input_plist,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)SQBO,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)INPUT,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)TAKES,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)ENDDEF,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)ID,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)MODULE,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)DEF,term);
	g->next=G[i].top;
	G[i].top=g;

	// -8- <ret> -> RETURNS SQBO <output_plist> SQBC SEMICOL
	i++;
	G[i].t=ret;
	g = createGrammarNode((G_element)(terminal)SEMICOL,term);
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)SQBC,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)output_plist,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)SQBO,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)RETURNS,term);
	g->next=G[i].top;
	G[i].top=g;

	// -9- <ret>. e 
	i++;
	G[i].t=ret;
	g= createGrammarNode((G_element)(terminal)e,term);
	G[i].top=g;

	// -10- <input_plist> -> ID COLON <dataType><N1> 
	i++;
	G[i].t=input_plist;
	g = createGrammarNode((G_element)(nonTerminal)N1,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)dataType,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)COLON,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)ID,term);
	g->next=G[i].top;
	G[i].top=g;

	// -11- <N1> -> COMMA ID COLON <dataType> <N1> 
	i++;
	G[i].t=N1;
	g = createGrammarNode((G_element)(nonTerminal)N1,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)dataType,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)COLON,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)ID,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)COMMA,term);
	g->next=G[i].top;
	G[i].top=g;

	// -12- <N1> -> e 
	i++;
	G[i].t=N1;
	g= createGrammarNode((G_element)(terminal)e,term);
	G[i].top=g;

	// -13- <output_plist> -> ID COLON <type> <N2>
	i++;
	G[i].t=output_plist;
	g = createGrammarNode((G_element)(nonTerminal)N2,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)type,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)COLON,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)ID,term);
	g->next=G[i].top;
	G[i].top=g;

	// -14- <N2> -> COMMA ID COLON <type><N2> 
	i++;
	G[i].t=N2;
	g = createGrammarNode((G_element)(nonTerminal)N2,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)type,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)COLON,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)ID,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)COMMA,term);
	g->next=G[i].top;
	G[i].top=g;

	// -15- <N2> -> e 
	i++;
	G[i].t=N2;
	g= createGrammarNode((G_element)(terminal)e,term);
	G[i].top=g;

	// -16- <dataType> -> INTEGER 
	i++;
	G[i].t=dataType;
	g= createGrammarNode((G_element)(terminal)INTEGER,term);
	G[i].top=g;

	// -17- <dataType> -> REAL 
	i++;
	G[i].t=dataType;
	g= createGrammarNode((G_element)(terminal)REAL,term);
	G[i].top=g;

	// -18- <dataType> -> BOOLEAN 
	i++;
	G[i].t=dataType;
	g= createGrammarNode((G_element)(terminal)BOOLEAN,term);
	G[i].top=g;

	// -19- <dataType> -> ARRAY SQBO <range> SQBC OF <type> 
	i++;
	G[i].t=dataType;
	g = createGrammarNode((G_element)(nonTerminal)type,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)OF,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)SQBC,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)range,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)SQBO,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)ARRAY,term);
	g->next=G[i].top;
	G[i].top=g;

	// -20- <type> -> INTEGER 
	i++;
	G[i].t=type;
	g= createGrammarNode((G_element)(terminal)INTEGER,term);
	G[i].top=g;

	// -21- <type> -> REAL
	i++;
	G[i].t=type;
	g= createGrammarNode((G_element)(terminal)REAL,term);
	G[i].top=g; 

	// -22- <type> -> BOOLEAN 
	i++;
	G[i].t=type;
	g= createGrammarNode((G_element)(terminal)BOOLEAN,term);
	G[i].top=g;

	// -23- <moduleDef> -> START <statements> END 
	i++;
	G[i].t=moduleDef;
	g= createGrammarNode((G_element)(terminal)END,term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)statements,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)START,term);
	g->next=G[i].top;
	G[i].top=g;

	// -24- <statements> -><statement> <statements> 
	i++;
	G[i].t=statements;
	g= createGrammarNode((G_element)(nonTerminal)statement,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)statements,non_term);
	G[i].top->next=g;

	// -25- <statements> -> e 
	i++;
	G[i].t=statements;
	g= createGrammarNode((G_element)(terminal)e,term);
	G[i].top=g;

	// -26-  <statement> -><ioStmt>
	i++;
	G[i].t=statement;
	g= createGrammarNode((G_element)(nonTerminal)ioStmt,non_term);
	G[i].top=g;

	// -27-  <statement> -><simpleStmt>
	i++;
	G[i].t=statement;
	g= createGrammarNode((G_element)(nonTerminal)simpleStmt,non_term);
	G[i].top=g;

	// -28- <statement> -><declareStmt>
	i++;
	G[i].t=statement;
	g= createGrammarNode((G_element)(nonTerminal)declareStmt,non_term);
	G[i].top=g;

	// -29- <statement> -><conditionalStmt>
	i++;
	G[i].t=statement;
	g= createGrammarNode((G_element)(nonTerminal)conditionalStmt,non_term);
	G[i].top=g;

	// -30- <statement> -><iterativeStmt> 
	i++;
	G[i].t=statement;
	g= createGrammarNode((G_element)(nonTerminal)iterativeStmt,non_term);
	G[i].top=g;

	// -31- <ioStmt> ->GET_VALUE BO ID BC SEMICOL 
	i++;
	G[i].t=ioStmt;
	g = createGrammarNode((G_element)(terminal)SEMICOL,term);
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)BC,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)ID,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)BO,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)GET_VALUE,term);
	g->next=G[i].top;
	G[i].top=g;

	// -32- <ioStmt> ->PRINT BO <var> BC SEMICOL 
	i++;
	G[i].t=ioStmt;
	g = createGrammarNode((G_element)(terminal)SEMICOL,term);
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)BC,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)var,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)BO,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)PRINT,term);
	g->next=G[i].top;
	G[i].top=g;

	// -33- <var> -> ID <whichID> 
	i++;
	G[i].t=var;
	g = createGrammarNode((G_element)(nonTerminal)whichID,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)ID,term);
	g->next=G[i].top;
	G[i].top=g;

	// -34- <var> -> NUM 
	i++;
	G[i].t=var;
	g= createGrammarNode((G_element)(terminal)NUM,term);
	G[i].top=g;

	// -35- <var> -> RNUM 
	i++;
	G[i].t=var;
	g= createGrammarNode((G_element)(terminal)RNUM,term);
	G[i].top=g;

	// -36- <whichID> -> SQBO ID SQBC 
	i++;
	G[i].t=whichID;
	g = createGrammarNode((G_element)(terminal)SQBC,term);
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)ID,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)SQBO,term);
	g->next=G[i].top;
	G[i].top=g;

	// -37-  <whichID> -> e 
	i++;
	G[i].t=whichID;
	g= createGrammarNode((G_element)(terminal)e,term);
	G[i].top=g;

	// -38-  <simpleStmt> -> <assignmentStmt> 
	i++;
	G[i].t=simpleStmt;
	g= createGrammarNode((G_element)(nonTerminal)assignmentStmt,non_term);
	G[i].top=g;

	// -39 - <simpleStmt> -> <moduleReuseStmt>
	i++;
	G[i].t=simpleStmt;
	g= createGrammarNode((G_element)(nonTerminal)moduleReuseStmt,non_term);
	G[i].top=g; 

	// -40- <assignmentStmt> -> ID <whichStmt>
	i++;
	G[i].t=assignmentStmt;
	g = createGrammarNode((G_element)(nonTerminal)whichStmt,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)ID,term);
	g->next=G[i].top;
	G[i].top=g;

	// -41- <whichStmt> -><lvalueIDStmt> 
	i++;
	G[i].t=whichStmt;
	g= createGrammarNode((G_element)(nonTerminal)lvalueIDStmt,non_term);
	G[i].top=g; 

	// -42- <whichStmt> -> <lvalueARRStmt> 
	i++;
	G[i].t=whichStmt;
	g= createGrammarNode((G_element)(nonTerminal)lvalueARRStmt,non_term);
	G[i].top=g; 

	// -43- <lvalueIDStmt> -> ASSIGNOP <expression> SEMICOL 
	i++;
	G[i].t=lvalueIDStmt;
	g = createGrammarNode((G_element)(terminal)SEMICOL,term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)expression,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)ASSIGNOP,term);
	g->next=G[i].top;
	G[i].top=g;

	// -44-  <lvalueARRStmt> -> SQBO <index_nt> SQBC ASSIGNOP <expression> SEMICOL 
	i++;
	G[i].t=lvalueARRStmt;
	g = createGrammarNode((G_element)(terminal)SEMICOL,term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)expression,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)ASSIGNOP,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)SQBC,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)index_nt,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)SQBO,term);
	g->next=G[i].top;
	G[i].top=g;

	// -45- <index_nt> -> NUM 
	i++;
	G[i].t=index_nt;
	g= createGrammarNode((G_element)(terminal)NUM,term);
	G[i].top=g;

	// -46- <index_nt> -> ID 
	i++;
	G[i].t=index_nt;
	g= createGrammarNode((G_element)(terminal)ID,term);
	G[i].top=g;

	// -47- <moduleReuseStmt> -><optional> USE MODULE ID WITH PARAMETERS <idList>SEMICOL 
	i++;
	G[i].t=moduleReuseStmt;
	g = createGrammarNode((G_element)(terminal)SEMICOL,term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)idList,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)PARAMETERS,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)WITH,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)ID,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)MODULE,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)USE,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)optional,non_term);
	g->next=G[i].top;
	G[i].top=g;

	// -48- <optional> -> SQBO <idList> SQBC ASSIGNOP 
	i++;
	G[i].t=optional;
	g = createGrammarNode((G_element)(terminal)ASSIGNOP,term);
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)SQBC,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)idList,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)SQBO,term);
	g->next=G[i].top;
	G[i].top=g;

	// -49- <optional> ->e 
	i++;
	G[i].t=optional;
	g= createGrammarNode((G_element)(terminal)e,term);
	G[i].top=g;

	// -50- <idList> -> ID <N3> 
	i++;
	G[i].t=idList;
	g = createGrammarNode((G_element)(nonTerminal)N3,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)ID,term);
	g->next=G[i].top;
	G[i].top=g;

	// -51- <N3> -> COMMA ID <N3> 
	i++;
	G[i].t=N3;
	g = createGrammarNode((G_element)(nonTerminal)N3,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)ID,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)COMMA,term);
	g->next=G[i].top;
	G[i].top=g;

	// -52- <N3> -> e 
	i++;
	G[i].t=N3;
	g= createGrammarNode((G_element)(terminal)e,term);
	G[i].top=g;

	// -53- <expression>-> <arithmeticOrBooleanExpr> 
	i++;
	G[i].t=expression;
	g= createGrammarNode((G_element)(nonTerminal)arithmeticOrBooleanExpr,non_term);
	G[i].top=g;

	// -54- <expression>-> MINUS BO <arithmeticExpr> BC
	i++;
	G[i].t=expression;
	g = createGrammarNode((G_element)(terminal)BC,term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)arithmeticExpr,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)BO,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)MINUS,term);
	g->next=G[i].top;
	G[i].top=g;

	// -55- <arithmeticOrBooleanExpr>-> <AnyTerm> <N7>  
	i++;
	G[i].t=arithmeticOrBooleanExpr;
	g= createGrammarNode((G_element)(nonTerminal)N7,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)AnyTerm,non_term);
	g->next=G[i].top;
	G[i].top=g;

	// -56- <arithmeticOrBooleanExpr>-> BO <arithmeticOrBooleanExpr> BC 
	/*i++;
	G[i].t=arithmeticOrBooleanExpr;
	g = createGrammarNode((G_element)(terminal)BC,term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)arithmeticOrBooleanExpr,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)BO,term);
	g->next=G[i].top;
	G[i].top=g;
	*/
	// -57- <N7> -> <logicalOp> <AnyTerm> <N7> 
	i++;
	G[i].t=N7;
	g= createGrammarNode((G_element)(nonTerminal)N7,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)AnyTerm,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)logicalOp,non_term);
	g->next=G[i].top;
	G[i].top=g;

	// -58- <N7> -> e
	i++;
	G[i].t=N7;
	g= createGrammarNode((G_element)(terminal)e,term);
	G[i].top=g;

	// -59- <AnyTerm>-> <arithmeticExpr> <N8>  
	i++;
	G[i].t=AnyTerm;
	g= createGrammarNode((G_element)(nonTerminal)N8,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)arithmeticExpr,non_term);
	g->next=G[i].top;
	G[i].top=g;

	// -60- <N8> -> <relationalOp> <arithmeticExpr><N8> 
	i++;
	G[i].t=N8;
	g= createGrammarNode((G_element)(nonTerminal)N8,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)arithmeticExpr,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)relationalOp,non_term);
	g->next=G[i].top;
	G[i].top=g;

	// -61- <N8> -> e 
	i++;
	G[i].t=N8;
	g= createGrammarNode((G_element)(terminal)e,term);
	G[i].top=g;

	// -62- <arithmeticExpr> -><term_1> <N4> 
	i++;
	G[i].t=arithmeticExpr;
	g= createGrammarNode((G_element)(nonTerminal)N4,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)term_1,non_term);
	g->next=G[i].top;
	G[i].top=g;

	// -63- <N4> -> <op1> <term_1> <N4> 
	i++;
	G[i].t=N4;
	g= createGrammarNode((G_element)(nonTerminal)N4,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)term_1,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)op1,non_term);
	g->next=G[i].top;
	G[i].top=g;

	// -64- <N4> -> e  
	i++;
	G[i].t=N4;
	g= createGrammarNode((G_element)(terminal)e,term);
	G[i].top=g;

	// -65- <term_1> -> <factor> <N5>
	i++;
	G[i].t=term_1;
	g= createGrammarNode((G_element)(nonTerminal)N5,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)factor,non_term);
	g->next=G[i].top;
	G[i].top=g; 

	// -66- <N5> -> <op2> <factor> <N5>
	i++;
	G[i].t=N5;
	g= createGrammarNode((G_element)(nonTerminal)N5,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)factor,non_term);
	g->next=G[i].top;
	G[i].top=g; 
	g= createGrammarNode((G_element)(nonTerminal)op2,non_term);
	g->next=G[i].top;
	G[i].top=g;

	// -67- <N5> -> e 
	i++;
	G[i].t=N5;
	g= createGrammarNode((G_element)(terminal)e,term);
	G[i].top=g;

	// -68- <factor> -><var> 
	i++;
	G[i].t=factor;
	g= createGrammarNode((G_element)(nonTerminal)var,non_term);
	G[i].top=g;

	// -69- <op1>->PLUS
	i++;
	G[i].t=op1;
	g= createGrammarNode((G_element)(terminal)PLUS,term);
	G[i].top=g;

	// -70- <op1>->MINUS 
	i++;
	G[i].t=op1;
	g= createGrammarNode((G_element)(terminal)MINUS,term);
	G[i].top=g;

	// -71- <op2>->MUL
	i++;
	G[i].t=op2;
	g= createGrammarNode((G_element)(terminal)MUL,term);
	G[i].top=g;

	// -73- <op2>->DIV  
	i++;
	G[i].t=op2;
	g= createGrammarNode((G_element)(terminal)DIV,term);
	G[i].top=g;

	// -74- <logicalOp> ->AND 
	i++;
	G[i].t=logicalOp;
	g= createGrammarNode((G_element)(terminal)AND,term);
	G[i].top=g;

	// -75- <logicalOp> -> OR 
	i++;
	G[i].t=logicalOp;
	g= createGrammarNode((G_element)(terminal)OR,term);
	G[i].top=g;

	// -76- <relationalOp> -> LT 
	i++;
	G[i].t=relationalOp;
	g= createGrammarNode((G_element)(terminal)LT,term);
	G[i].top=g;

	// -77- <relationalOp> -> LE 
	i++;
	G[i].t=relationalOp;
	g= createGrammarNode((G_element)(terminal)LE,term);
	G[i].top=g;

	// -78- <relationalOp> -> GT 
	i++;
	G[i].t=relationalOp;
	g= createGrammarNode((G_element)(terminal)GT,term);
	G[i].top=g;

	// -79- <relationalOp> -> GE 
	i++;
	G[i].t=relationalOp;
	g= createGrammarNode((G_element)(terminal)GE,term);
	G[i].top=g;

	// -80- <relationalOp> -> EQ 
	i++;
	G[i].t=relationalOp;
	g= createGrammarNode((G_element)(terminal)EQ,term);
	G[i].top=g;

	// -81- <relationalOp> -> NE 
	i++;
	G[i].t=relationalOp;
	g= createGrammarNode((G_element)(terminal)NE,term);
	G[i].top=g;

	// -82- <declareStmt> -> DECLARE <idList> COLON <dataType> SEMICOL 
	i++;
	G[i].t=declareStmt;
	g = createGrammarNode((G_element)(terminal)SEMICOL,term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)dataType,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)COLON,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)idList,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)DECLARE,term);
	g->next=G[i].top;
	G[i].top=g;

	// -83- <conditionalStmt> ->SWITCH BO ID BC START <caseStmts><default_1> END 
	i++;
	G[i].t=conditionalStmt;
	g = createGrammarNode((G_element)(terminal)END,term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)default_1,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)caseStmts,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)START,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)BC,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)ID,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)BO,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)SWITCH,term);
	g->next=G[i].top;
	G[i].top=g;

	// -84- <caseStmts> -> CASE <value> COLON <statements> BREAK SEMICOL <N9> 
	i++;
	G[i].t=caseStmts;
	g= createGrammarNode((G_element)(nonTerminal)N9,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)SEMICOL,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)BREAK,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)statements,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)COLON,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)value,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)CASE,term);
	g->next=G[i].top;
	G[i].top=g;

	// -85-  <N9> -> CASE <value> COLON <statements> BREAK SEMICOL <N9> 
	i++;
	G[i].t=N9;
	g= createGrammarNode((G_element)(nonTerminal)N9,non_term);
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)SEMICOL,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)BREAK,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)statements,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)COLON,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)value,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)CASE,term);
	g->next=G[i].top;
	G[i].top=g;

	// -86-  <N9> -> e 
	i++;
	G[i].t=N9;
	g= createGrammarNode((G_element)(terminal)e,term);
	G[i].top=g;

	// -87- <value> -> NUM 
	i++;
	G[i].t=value;
	g= createGrammarNode((G_element)(terminal)NUM,term);
	G[i].top=g;

	// -88- <value> -> TRUE_ 
	i++;
	G[i].t=value;
	g= createGrammarNode((G_element)(terminal)TRUE_,term);
	G[i].top=g;

	// -89- <value> -> FALSE_ 
	i++;
	G[i].t=value;
	g= createGrammarNode((G_element)(terminal)FALSE_,term);
	G[i].top=g;

	// -90- <default_1> ->DEFAULT COLON <statements> BREAK SEMICOL 
	i++;
	G[i].t=default_1;
	g = createGrammarNode((G_element)(terminal)SEMICOL,term);
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)BREAK,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)statements,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)COLON,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)DEFAULT,term);
	g->next=G[i].top;
	G[i].top=g;

	// -91- <default_1> ->e
	i++;
	G[i].t=default_1;
	g= createGrammarNode((G_element)(terminal)e,term);
	G[i].top=g;

	// -92- <iterativeStmt> -> FOR BO ID IN <range> BC START <statements> END 
	i++;
	G[i].t=iterativeStmt;
	g = createGrammarNode((G_element)(terminal)END,term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)statements,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)START,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)BC,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)range,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)IN,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)ID,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)BO,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)FOR,term);
	g->next=G[i].top;
	G[i].top=g;

	// -93- <iterativeStmt> -> WHILE BO <expression> BC START <statements> END 
	i++;
	G[i].t=iterativeStmt;
	g = createGrammarNode((G_element)(terminal)END,term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)statements,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)START,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)BC,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)expression,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)BO,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)WHILE,term);
	g->next=G[i].top;
	G[i].top=g;

	// -94- <range> ->NUM RANGEOP NUM 
	i++;
	G[i].t=range;
	g = createGrammarNode((G_element)(terminal)NUM,term);
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)RANGEOP,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)NUM,term);
	g->next=G[i].top;
	G[i].top=g;
	// -95- <expression>-> PLUS BO <arithmeticExpr> BC
	i++;
	G[i].t=expression;
	g = createGrammarNode((G_element)(terminal)BC,term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)arithmeticExpr,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)BO,term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)PLUS,term);
	g->next=G[i].top;
	G[i].top=g;
	// -96- <factor> ->BO <arithmeticOrBooleanExpr> BC
	i++;
	G[i].t=factor;
	g = createGrammarNode((G_element)(terminal)BC,term);
	G[i].top=g;
	g= createGrammarNode((G_element)(nonTerminal)arithmeticOrBooleanExpr,non_term);
	g->next=G[i].top;
	G[i].top=g;
	g= createGrammarNode((G_element)(terminal)BO,term);
	g->next=G[i].top;
	G[i].top=g;
	return G;
}

void createFirstSets()
{
	int i=0,j;
	for (i=0;i<51;++i)
	 for (j=0;j<15;++j)
	  firstSet[i].first[j]=(terminal)-1;
	i=0;

	// i=0, Program
	firstSet[i].first[0]=DECLARE;
	firstSet[i].first[1]=DEF;
	firstSet[i].first[2]=DRIVERDEF;
			
	// 1 <moduleDeclarations>
	 ++i;
	 firstSet[i].first[0]=DECLARE;
	firstSet[i].first [1]= e;

	// 2 <moduleDeclaration> 
	++i;
	firstSet[i].first[0]=DECLARE ;

	// 3<otherModules> 
	++i;
	firstSet[i].first[0]= DEF;
	firstSet[i].first[1]= e;

	// 4 <driverModule> 
	++i;
	firstSet[i].first[0]=DRIVERDEF ;

	// 5 <module> 
	++i;
	firstSet[i].first[0]=DEF ;

	// 6 <ret> 
	++i;
	firstSet[i].first[0]=RETURNS;
	firstSet[i].first[1]= e;

	// 7 <input_plist> 
	++i;
	firstSet[i].first[0]= ID ;

	// 8 <N1> 
	++i;
	firstSet[i].first[0]=COMMA ;
	firstSet[i].first[1]= e;

	// 9 <output_plist> 
	++i;
	firstSet[i].first[0]= ID ;

	//10 <N2> 
	++i;
	firstSet[i].first[0]=COMMA ;
	firstSet[i].first[1]= e;

	// 11 <dataType> 
	++i;
	firstSet[i].first[0]= INTEGER;
	firstSet[i].first[1]= REAL;
	firstSet[i].first[2]=BOOLEAN ;
	firstSet[i].first[3]=ARRAY; 

	// 12 <type>
	++i;
	firstSet[i].first[0]=INTEGER;
	firstSet[i].first[1]=REAL;
	firstSet[i].first[2]=BOOLEAN;

	// 13 <moduleDef> 
	++i;
	firstSet[i].first[0]=START; 

	// 14 <statements> 
	++i;
	firstSet[i].first[0]=GET_VALUE;
	firstSet[i].first[1]= PRINT;
	firstSet[i].first[2]= ID;
	firstSet[i].first[3]= SQBO;
	firstSet[i].first[4]= USE;
	firstSet[i].first[5]=DECLARE;
	firstSet[i].first[6]=SWITCH; 
	firstSet[i].first[7]=FOR;
	firstSet[i].first[8]=WHILE;
	firstSet[i].first[9]=e;

	// 15 <statement>
	++i;
	firstSet[i].first[0]=GET_VALUE;
	firstSet[i].first[1]= PRINT;
	firstSet[i].first[2]= ID;
	firstSet[i].first[3]= SQBO;
	firstSet[i].first[4]= USE;
	firstSet[i].first[5]=DECLARE;
	firstSet[i].first[6]=SWITCH; 
	firstSet[i].first[7]=FOR;
	firstSet[i].first[8]=WHILE;
	 
	// 16 <ioStmt> 
	++i;
	firstSet[i].first[0]=GET_VALUE;
	firstSet[i].first[1]= PRINT;

	// 17 <var> 
	++i;
	firstSet[i].first[0]=ID ;
	firstSet[i].first[1]=NUM;
	firstSet[i].first[2]= RNUM;
	 
	// 18 <whichID> 
	++i;
	firstSet[i].first[0]=SQBO;
	firstSet[i].first[1]=e;
	 
	// 19 <simpleStmt> 
	++i;
	firstSet[i].first[0]=ID;
	firstSet[i].first[1]=SQBO;
	firstSet[i].first[2]=USE;

	// 20 <assignmentStmt>
	++i;
	firstSet[i].first[0]=ID;
	 
	// 21 <whichStmt>
	++i;
	firstSet[i].first[0]=ASSIGNOP;
	firstSet[i].first[1]=SQBO;

	// 22 <lvalueIDStmt>
	++i ;
	firstSet[i].first[0]=ASSIGNOP;
	 
	// 23 <lvalueARRStmt> 
	++i;
	firstSet[i].first[0]=SQBO ;

	// 24 <index_nt> 
	++i;
	firstSet[i].first[0]=NUM;
	firstSet[i].first[1]=ID;
	 
	// 25 <moduleReuseStmt>
	++i;
	firstSet[i].first[0]=USE;
	firstSet[i].first[1]=SQBO;

	// 26 <optional>
	++i;
	firstSet[i].first[0]=SQBO;
	firstSet[i].first[1]=e;

	// 27 <idList>
	++i;
	firstSet[i].first[0]=ID;
	 
	// 28 <N3>
	++i;
	firstSet[i].first[0]=COMMA;
	firstSet[i].first[1]=e ;

	// 29 <expression>
	++i;
	firstSet[i].first[0]=MINUS;
	firstSet[i].first[1]=BO;
	firstSet[i].first[2]=ID;
	firstSet[i].first[3]=NUM;
	firstSet[i].first[4]=RNUM;
	firstSet[i].first[5]=PLUS;
	 
	// 30 <arithmeticOrBooleanExpr>
	++i;
	firstSet[i].first[0]=BO;
	firstSet[i].first[1]=ID;
	firstSet[i].first[2]=NUM;
	firstSet[i].first[3]=RNUM;

	// 31 <N7>
	++i;
	firstSet[i].first[0]=AND;
	firstSet[i].first[1]=OR;
	firstSet[i].first[2]=e;

	// 32 <AnyTerm>
	++i;
	firstSet[i].first[0]=ID;
	firstSet[i].first[1]=NUM; 
	firstSet[i].first[2]=RNUM;
	firstSet[i].first[3]=BO;
	 
	// 33 <N8>
	++i;
	firstSet[i].first[0]=LT;
	firstSet[i].first[1]=LE;
	firstSet[i].first[2]=GT;
	firstSet[i].first[3]=GE;
	firstSet[i].first[4]=EQ;
	firstSet[i].first[5]=NE;
	firstSet[i].first[6]=e;

	// 34 <arithmeticExpr>
	++i;
	firstSet[i].first[0]=ID;
	firstSet[i].first[1]=NUM;
	firstSet[i].first[2]=RNUM;
	firstSet[i].first[3]=BO;

	// 35 <N4>
	++i;
	firstSet[i].first[0]=PLUS;
	firstSet[i].first[1]=MINUS;
	firstSet[i].first[2]=e;

	// 36 <term_1>
	++i;
	firstSet[i].first[0]=ID;
	firstSet[i].first[1]=NUM;
	firstSet[i].first[2]=RNUM;
	firstSet[i].first[3]=BO;

	// 37 <N5>
	++i;
	firstSet[i].first[0]=MUL;
	firstSet[i].first[1]=DIV;
	firstSet[i].first[2]=e;

	// 38 <factor> 
	++i;
	firstSet[i].first[0]=ID;
	firstSet[i].first[1]=NUM;
	firstSet[i].first[2]=RNUM;
	firstSet[i].first[3]=BO;

	// 39 <op1>
	++i;
	firstSet[i].first[0]=PLUS;
	firstSet[i].first[1]=MINUS;

	// 40 <op2>
	++i;
	firstSet[i].first[0]=MUL;
	firstSet[i].first[1]=DIV;

	// 41 <logicalOp> 
	++i;
	firstSet[i].first[0]=AND;
	firstSet[i].first[1]=OR;

	// 42 <relationalOp>
	++i;
	firstSet[i].first[0]=LT;
	firstSet[i].first[1]=LE;
	firstSet[i].first[2]=GT;
	firstSet[i].first[3]=GE;
	firstSet[i].first[4]=EQ;
	firstSet[i].first[5]=NE;
	 
	// 43 <declareStmt>
	++i;
	firstSet[i].first[0]=DECLARE;
	 
	// 44 <conditionalStmt>
	++i;
	firstSet[i].first[0]=SWITCH;
	 
	// 45 <caseStmts>
	++i;
	firstSet[i].first[0]=CASE;
	 
	// 46 <N9>
	++i;
	firstSet[i].first[0]=CASE;
	firstSet[i].first[1]=e;

	// 47 <value>
	++i;
	firstSet[i].first[0]=NUM;
	firstSet[i].first[1]=TRUE_;
	firstSet[i].first[2]=FALSE_;
	 
	// 48 <default_1>
	++i;
	firstSet[i].first[0]=DEFAULT;
	firstSet[i].first[1]=e;

	// 49 <iterativeStmt>
	++i;
	firstSet[i].first[0]=FOR; 
	firstSet[i].first[1]=WHILE;
	 
	// 50 <range>
	++i;
	firstSet[i].first[0]=NUM;
	 
	}

	void createFollowSets()
	{ int i, j;
	i=0;
	for (i=0;i<51;++i)
	 for (j=0;j<15;++j)
	  followSet[i].follow[j]=(terminal)-1;
	i=0;

	// i=0 program
	followSet[i].follow[0]=$;

	// 1 ModuleDeclarations
	i++;
	followSet[i].follow[0]=DEF;
	followSet[i].follow[1]=DRIVERDEF;

	// 2 <moduleDeclaration> 
	i++;
	followSet[i].follow[0]=DECLARE;
	followSet[i].follow[1]=DEF;
	followSet[i].follow[2]=DRIVERDEF;

	// 3 <otherModules> 
	i++;
	followSet[i].follow[0]=DRIVERDEF;
	followSet[i].follow[1]=$;

	// 4 <driverModule> 
	i++;
	followSet[i].follow[0]=DEF;
	followSet[i].follow[1]=$;

	// 5 <module>
	i++;
	followSet[i].follow[0]=DEF;
	followSet[i].follow[1]=DRIVERDEF;
	followSet[i].follow[2]=$;

	// 6 <ret>
	i++;
	followSet[i].follow[0]=START;

	// 7 <input_plist>
	i++;
	followSet[i].follow[0]=SQBC;

	// 8 <N1>
	i++;
	followSet[i].follow[0]=SQBC;

	// 9 <output_plist>
	i++;
	followSet[i].follow[0]=SQBC;

	// 10 <N2>
	i++;
	followSet[i].follow[0]=SQBC;

	// 11 <dataType>
	i++;
	followSet[i].follow[0]=COMMA;
	followSet[i].follow[1]=SQBC;
	followSet[i].follow[2]=SEMICOL;

	// 12 <type>
	i++;
	followSet[i].follow[0]=COMMA;
	followSet[i].follow[1]=SQBC;
	followSet[i].follow[2]=SEMICOL;

	// 13 <moduleDef>
	i++;
	followSet[i].follow[0]=DEF;
	followSet[i].follow[1]=DRIVERDEF;
	followSet[i].follow[2]=$;

	// 14 <statements>
	i++;
	followSet[i].follow[0]=END;
	followSet[i].follow[1]=BREAK;

	// 15 <statement>
	i++;
	followSet[i].follow[0]=GET_VALUE;
	followSet[i].follow[1]=PRINT;
	followSet[i].follow[2]=ID;
	followSet[i].follow[3]=SQBO;
	followSet[i].follow[4]=USE;
	followSet[i].follow[5]=DECLARE;
	followSet[i].follow[6]=SWITCH;
	followSet[i].follow[7]=FOR;
	followSet[i].follow[8]=WHILE;
	followSet[i].follow[9]=END;
	followSet[i].follow[10]=BREAK;

	// 16 <ioStmt>
	i++;
	followSet[i].follow[0]=GET_VALUE;
	followSet[i].follow[1]=PRINT;
	followSet[i].follow[2]=ID;
	followSet[i].follow[3]=SQBO;
	followSet[i].follow[4]=USE;
	followSet[i].follow[5]=DECLARE;
	followSet[i].follow[6]=SWITCH;
	followSet[i].follow[7]=FOR;
	followSet[i].follow[8]=WHILE;
	followSet[i].follow[9]=END;
	followSet[i].follow[10]=BREAK;

	// 17 <var> 
	i++;
	followSet[i].follow[0]=MUL;
	followSet[i].follow[1]=DIV;
	followSet[i].follow[2]=BC;
	followSet[i].follow[3]=LT;
	followSet[i].follow[4]=LE;
	followSet[i].follow[5]=GT;
	followSet[i].follow[6]=GE;
	followSet[i].follow[7]=EQ;
	followSet[i].follow[8]=NE;
	followSet[i].follow[9]=AND;
	followSet[i].follow[10]=OR;
	followSet[i].follow[11]=SEMICOL;
	followSet[i].follow[12]=PLUS;
	followSet[i].follow[13]=MINUS;

	// 18 <whichID>
	i++;
	followSet[i].follow[0]=MUL;
	followSet[i].follow[1]=DIV;
	followSet[i].follow[2]=BC;
	followSet[i].follow[3]=LT;
	followSet[i].follow[4]=LE;
	followSet[i].follow[5]=GT;
	followSet[i].follow[6]=GE;
	followSet[i].follow[7]=EQ;
	followSet[i].follow[8]=NE;
	followSet[i].follow[9]=AND;
	followSet[i].follow[10]=OR;
	followSet[i].follow[11]=SEMICOL;
	followSet[i].follow[12]=PLUS;
	followSet[i].follow[13]=MINUS;

	// 19 <simpleStmt> 
	i++;
	followSet[i].follow[0]=GET_VALUE;
	followSet[i].follow[1]=PRINT;
	followSet[i].follow[2]=ID;
	followSet[i].follow[3]=SQBO;
	followSet[i].follow[4]=USE;
	followSet[i].follow[5]=DECLARE;
	followSet[i].follow[6]=SWITCH;
	followSet[i].follow[7]=FOR;
	followSet[i].follow[8]=WHILE;
	followSet[i].follow[9]=END;
	followSet[i].follow[10]=BREAK;

	// 20 <assignmentStmt>
	i++;
	followSet[i].follow[0]=GET_VALUE;
	followSet[i].follow[1]=PRINT;
	followSet[i].follow[2]=ID;
	followSet[i].follow[3]=SQBO;
	followSet[i].follow[4]=USE;
	followSet[i].follow[5]=DECLARE;
	followSet[i].follow[6]=SWITCH;
	followSet[i].follow[7]=FOR;
	followSet[i].follow[8]=WHILE;
	followSet[i].follow[9]=END;
	followSet[i].follow[10]=BREAK;

	// 21 <whichStmt>
	i++;
	followSet[i].follow[0]=GET_VALUE;
	followSet[i].follow[1]=PRINT;
	followSet[i].follow[2]=ID;
	followSet[i].follow[3]=SQBO;
	followSet[i].follow[4]=USE;
	followSet[i].follow[5]=DECLARE;
	followSet[i].follow[6]=SWITCH;
	followSet[i].follow[7]=FOR;
	followSet[i].follow[8]=WHILE;
	followSet[i].follow[9]=END;

	// 22 <lvalueIDStmt>
	i++;
	followSet[i].follow[0]=GET_VALUE;
	followSet[i].follow[1]=PRINT;
	followSet[i].follow[2]=ID;
	followSet[i].follow[3]=SQBO;
	followSet[i].follow[4]=USE;
	followSet[i].follow[5]=DECLARE;
	followSet[i].follow[6]=SWITCH;
	followSet[i].follow[7]=FOR;
	followSet[i].follow[8]=WHILE;
	followSet[i].follow[9]=END;
	followSet[i].follow[10]=BREAK;

	// 23 <lvalueARRStmt>
	i++;
	followSet[i].follow[0]=GET_VALUE;
	followSet[i].follow[1]=PRINT;
	followSet[i].follow[2]=ID;
	followSet[i].follow[3]=SQBO;
	followSet[i].follow[4]=USE;
	followSet[i].follow[5]=DECLARE;
	followSet[i].follow[6]=SWITCH;
	followSet[i].follow[7]=FOR;
	followSet[i].follow[8]=WHILE;
	followSet[i].follow[9]=END;
	followSet[i].follow[10]=BREAK;

	// 24 <index>
	i++;
	followSet[i].follow[13]=SQBC;

	// 25 <moduleReuseStmt> 
	i++;
	followSet[i].follow[0]=GET_VALUE;
	followSet[i].follow[1]=PRINT;
	followSet[i].follow[2]=ID;
	followSet[i].follow[3]=SQBO;
	followSet[i].follow[4]=USE;
	followSet[i].follow[5]=DECLARE;
	followSet[i].follow[6]=SWITCH;
	followSet[i].follow[7]=FOR;
	followSet[i].follow[8]=WHILE;
	followSet[i].follow[9]=END;
	followSet[i].follow[10]=BREAK;

	// 26 <optional> 
	i++;
	followSet[i].follow[0]=USE;

	// 27 <idList>
	i++;
	followSet[i].follow[0]=SQBC;
	followSet[i].follow[1]=SEMICOL;
	followSet[i].follow[2]=COLON;

	// 28 <N3>
	i++;
	followSet[i].follow[0]=SQBC;
	followSet[i].follow[1]=SEMICOL;
	followSet[i].follow[2]=COLON;

	// 29 <expression>
	i++;
	followSet[i].follow[0]=SEMICOL;
	followSet[i].follow[1]=BC;

	// 30 <arithmeticOrBooleanExpr>
	i++;
	followSet[i].follow[0]=SEMICOL;
	followSet[i].follow[1]=BC;
	 
	// 31 <N7>
	i++;
	followSet[i].follow[0]=SEMICOL;
	followSet[i].follow[1]=BC;
	 
	// 32 <AnyTerm>
	i++;
	followSet[i].follow[0]=AND;
	followSet[i].follow[1]=OR;
	followSet[i].follow[2]=SEMICOL;
	followSet[i].follow[3]=BC;
	 
	// 33 <N8>
	i++;
	followSet[i].follow[0]=AND;
	followSet[i].follow[1]=OR;
	followSet[i].follow[2]=SEMICOL;
	followSet[i].follow[3]=BC;

	// 34 <arithmeticExpr>
	i++;
	followSet[i].follow[0]=BC;
	followSet[i].follow[1]=LT;
	followSet[i].follow[2]=LE;
	followSet[i].follow[3]=GT;
	followSet[i].follow[4]=GE;
	followSet[i].follow[5]=EQ;
	followSet[i].follow[6]=NE;
	followSet[i].follow[7]=AND;
	followSet[i].follow[8]=OR;
	followSet[i].follow[9]=SEMICOL;

	// 35 <N4>
	i++;
	followSet[i].follow[0]=BC;
	followSet[i].follow[1]=LT;
	followSet[i].follow[2]=LE;
	followSet[i].follow[3]=GT;
	followSet[i].follow[4]=GE;
	followSet[i].follow[5]=EQ;
	followSet[i].follow[6]=NE;
	followSet[i].follow[7]=AND;
	followSet[i].follow[8]=OR;
	followSet[i].follow[9]=SEMICOL;

	// 36 <term_1>
	i++;
	followSet[i].follow[0]=BC;
	followSet[i].follow[1]=LT;
	followSet[i].follow[2]=LE;
	followSet[i].follow[3]=GT;
	followSet[i].follow[4]=GE;
	followSet[i].follow[5]=EQ;
	followSet[i].follow[6]=NE;
	followSet[i].follow[7]=AND;
	followSet[i].follow[8]=OR;
	followSet[i].follow[9]=SEMICOL;
	followSet[i].follow[10]=PLUS;
	followSet[i].follow[11]=MINUS;

	// 37 <N5> 
	i++;
	followSet[i].follow[0]=BC;
	followSet[i].follow[1]=LT;
	followSet[i].follow[2]=LE;
	followSet[i].follow[3]=GT;
	followSet[i].follow[4]=GE;
	followSet[i].follow[5]=EQ;
	followSet[i].follow[6]=NE;
	followSet[i].follow[7]=AND;
	followSet[i].follow[8]=OR;
	followSet[i].follow[9]=SEMICOL;
	followSet[i].follow[10]=PLUS;
	followSet[i].follow[11]=MINUS;

	// 38 <factor> 
	i++;
	followSet[i].follow[0]=BC;
	followSet[i].follow[1]=LT;
	followSet[i].follow[2]=LE;
	followSet[i].follow[3]=GT;
	followSet[i].follow[4]=GE;
	followSet[i].follow[5]=EQ;
	followSet[i].follow[6]=NE;
	followSet[i].follow[7]=AND;
	followSet[i].follow[8]=OR;
	followSet[i].follow[9]=SEMICOL;
	followSet[i].follow[10]=PLUS;
	followSet[i].follow[11]=MINUS;
	followSet[i].follow[12]=MUL;
	followSet[i].follow[13]=DIV;

	// 39 <op1>
	i++;
	followSet[i].follow[0]=ID;
	followSet[i].follow[1]=NUM;
	followSet[i].follow[2]=RNUM;
	followSet[i].follow[3]=BO;
	 
	// 40 <op2>
	i++;
	followSet[i].follow[0]=ID;
	followSet[i].follow[1]=NUM;
	followSet[i].follow[2]=RNUM;
	followSet[i].follow[3]=BO;
	// 41 <logicalOp>
	i++;
	followSet[i].follow[0]=ID;
	followSet[i].follow[1]=NUM;
	followSet[i].follow[2]=RNUM;
	followSet[i].follow[3]=BO;
	// 42 <relationalOp>
	i++;
	followSet[i].follow[0]=ID;
	followSet[i].follow[1]=NUM;
	followSet[i].follow[2]=RNUM;
	followSet[i].follow[3]=BO;
	// 43 <declareStmt>
	i++;
	followSet[i].follow[0]=GET_VALUE;
	followSet[i].follow[1]=PRINT;
	followSet[i].follow[2]=ID;
	followSet[i].follow[3]=SQBO;
	followSet[i].follow[4]=USE;
	followSet[i].follow[5]=DECLARE;
	followSet[i].follow[6]=SWITCH;
	followSet[i].follow[7]=FOR;
	followSet[i].follow[8]=WHILE;
	followSet[i].follow[9]=END;
	followSet[i].follow[10]=BREAK;

	// 44 <conditionalStmt>
	i++;
	followSet[i].follow[0]=GET_VALUE;
	followSet[i].follow[1]=PRINT;
	followSet[i].follow[2]=ID;
	followSet[i].follow[3]=SQBO;
	followSet[i].follow[4]=USE;
	followSet[i].follow[5]=DECLARE;
	followSet[i].follow[6]=SWITCH;
	followSet[i].follow[7]=FOR;
	followSet[i].follow[8]=WHILE;
	followSet[i].follow[9]=END;
	followSet[i].follow[10]=BREAK;

	// 45 <caseStmts> 
	i++;
	followSet[i].follow[0]=DEFAULT;
	followSet[i].follow[1]=END;

	// 46 <N9>
	i++;
	followSet[i].follow[0]=DEFAULT;
	followSet[i].follow[1]=END;

	// 47 <value>
	i++;
	followSet[i].follow[0]=COLON;

	// 48 <default_1>
	i++;
	followSet[i].follow[0]=END;

	// 49 <iterativeStmt>
	i++;
	followSet[i].follow[0]=GET_VALUE;
	followSet[i].follow[1]=PRINT;
	followSet[i].follow[2]=ID;
	followSet[i].follow[3]=SQBO;
	followSet[i].follow[4]=USE;
	followSet[i].follow[5]=DECLARE;
	followSet[i].follow[6]=SWITCH;
	followSet[i].follow[7]=FOR;
	followSet[i].follow[8]=WHILE;
	followSet[i].follow[9]=END;
	followSet[i].follow[10]=BREAK;

	// 50 <range> 
	i++;
	followSet[i].follow[0]=BC;
	followSet[i].follow[1]=SQBC;

}

void createParseTable(Grammar G)   //using a universal table T instead of passing as an argument
{
	initializeParseTable();
	int i;
	for (i=0;i<GRAMCONST;++i)
	 {
	  if (G[i].top->t==term)
		if (G[i].top->ge.t!=(terminal)e)
		   T[(int)G[i].t][(int)G[i].top->ge.t]=i;
		else
		   fillfollow(G[i].t,i,G);
	  else  	  
		{fillForFirst(G[i].top, i,G);}
	 }
	//T[(nonTerminal)statements][(terminal)END]=25;
}

void fillForFirst(gnode n, int ind, Grammar G)
{ 
  int i;
  for (i=0;i<15;++i)
  {
  if (firstSet[(int)n->ge.nt].first[i]==(terminal)(-1))
      i=16; //end loop   
   else
      if (firstSet[(int)n->ge.nt].first[i]==(terminal)e)
	     if (n->next==NULL)
		     fillfollow(G[ind].t,ind, G);
		 else 
		    if (n->next->t==term)
		       T[(int)G[ind].t][(int)n->next->ge.t]=ind;             
	        else
		        fillForFirst(n->next,ind, G);
	  else
	     T[(int)G[ind].t][(int)firstSet[(int)n->ge.nt].first[i]]=ind;

  }
}

void fillfollow(nonTerminal nt, int ind, Grammar G)
{
  int i;
  for (i=0;i<15;++i)
   { if (followSet[(int)nt].follow[i]==(terminal)(-1))
      i=16; //end loop  
	 else
	  T[(int)G[ind].t][(int)followSet[(int)nt].follow[i]]=ind;
	}
}

void printParseTable()
{
	int nt,t;
	printf("\n");
	 for (nt=0;nt<51;nt++)
	   {for (t=0;t<57;t++)
		 {printf ("\t%d",T[nt][t]);}
		printf("\n");
	   }
}

void initializeParseTable()
{
	int nt,t;
	 for (nt=0;nt<51;nt++)
	   for (t=0;t<57;t++)
		  T[nt][t]=-1;
}


///////////////////////////////////////////////////////////////////

char * getToken(terminal t)
{ 
	char * s;
	switch ((int)t)
	{
	case (int)INTEGER: s="INTEGER";
		break;
	case (int)REAL: s="REAL";
		break;
	case (int)BOOLEAN: s="BOOLEAN";
		break;
	case (int)OF: s="OF";
		break;
	case (int)ARRAY: s="ARRAY";
		break;
	case (int)START: s="START";
		break;
	case (int)END: s="END";
		break;
	case (int)DECLARE: s="DECLARE";
		break;
	case (int)MODULE: s="MODULE";
		break;
	case (int)DRIVER: s="DRIVER";
		break;
	case (int)PROGRAM: s="PROGRAM";
		break;
	case (int)GET_VALUE: s="GET_VALUE";
		break;
	case (int)PRINT: s="PRINT";
		break;
	case (int)USE: s="USE";
		break;
	case (int)WITH: s="WITH";
		break;
	case (int)PARAMETERS: s="PARAMETERS";
		break;
	case (int)TRUE_: s="TRUE_";
		break;
	case (int)FALSE_: s="FALSE_";
		break;
	case (int)TAKES: s="TAKES";
		break;
	case (int)INPUT: s="INPUT";
		break;
	case (int)RETURNS: s="RETURNS";
		break;
	case (int)AND: s="AND";
		break;
	case (int)OR: s="OR";
		break;
	case (int)FOR: s="FOR";
		break;
	case (int)IN: s="IN";
		break;
	case (int)SWITCH: s="SWITCH";
		break;
	case (int)CASE: s="CASE";
		break;
	case (int)BREAK: s="BREAK";
		break;
	case (int)DEFAULT: s="DEFAULT";
		break;
	case (int)WHILE: s="WHILE";
		break;
	case (int)PLUS: s="PLUS";
		break;
	case (int)MINUS: s="MINUS";
		break;
	case (int)MUL: s="MUL";
		break;
	case (int)DIV: s="DIV";
		break;
	case (int)LT: s="LT";
		break;
	case (int)LE: s="LE";
		break;
	case (int)GE: s="GE";
		break;
	case (int)GT: s="GT";
		break;
	case (int)EQ: s="EQ";
		break;
	case (int)NE: s="NE";
		break;
	case (int)DEF: s="DEF";
		break;
	case (int)ENDDEF: s="ENDDEF";
		break;
	case (int)COLON: s="COLON";
		break;
	case (int)RANGEOP: s="RANGEOP";
		break;
	case (int)SEMICOL: s="SEMICOL";
		break;
	case (int)COMMA: s="COMMA";
		break;
	case (int)ASSIGNOP: s="ASSIGNOP";
		break;
	case (int)SQBO: s="SQBO";
		break;
	case (int)SQBC: s="SQBC";
		break;
	case (int)BO: s="BO";
		break;
	case (int)BC: s="BC";
		break;
	case (int)COMMENTMARK: s="COMMENTMARK";
		break;
	case (int)DRIVERENDDEF: s="DRIVERENDDEF";
		break;
	case (int)DRIVERDEF: s="DRIVERDEF";
		break;
	case (int)NUM: s="NUM";
		break;
	case (int)RNUM: s="RNUM";
		break;
	case (int)$: s="$";
		break;
	case (int)e: s="e";
		break;
	case (int)null_point: s="null_point";
		break;
	case (int)ID: s="ID";
		break;
	}
	return s;
}


char * getNON_term(nonTerminal nt)
{
	char * s;
	switch ((int)nt)
	{
	case (int)program: s="program";
			break;
	case (int)moduleDeclarations: s="moduleDeclarations";
			break;
	case (int)moduleDeclaration: s="moduleDeclaration";
			break;
	case (int)otherModules: s="otherModules";
			break;
	case (int)driverModule: s="driverModule";
			break;
	case (int)module: s="module";
			break;
	case (int)ret: s="ret";
			break;
	case (int)input_plist: s="input_plist";
			break;
	case (int)N1: s="N1";
			break;
	case (int)output_plist: s="output_plist";
			break;
	case (int)N2: s="N2";
			break;
	case (int)dataType: s="dataType";
			break;
	case (int)type: s="type";
			break;
	case (int)moduleDef: s="moduleDef";
			break;
	case (int)statements: s="statements";
			break;
	case (int)statement: s="statement";
			break;
	case (int)ioStmt: s="ioStmt";
			break;
	case (int)var: s="var";
			break;
	case (int)whichID: s="whichID";
			break;
	case (int)simpleStmt: s="simpleStmt";
			break;
	case (int)assignmentStmt: s="assignmentStmt";
			break;
	case (int)whichStmt: s="whichStmt";
			break;
	case (int)lvalueIDStmt: s="lvalueIDStmt";
			break;
	case (int)lvalueARRStmt: s="lvalueARRStmt";
			break;
	case (int)index_nt: s="index_nt";
			break;
	case (int)moduleReuseStmt: s="moduleReuseStmt";
			break;
	case (int)optional: s="optional";
			break;
	case (int)idList: s="idList";
			break;
	case (int)N3: s="N3";
			break;
	case (int)expression: s="expression";
			break;
	case (int)arithmeticOrBooleanExpr: s="arithmeticOrBooleanExpr";
			break;
	case (int)N7: s="N7";
			break;
	case (int)AnyTerm: s="AnyTerm";
			break;
	case (int)N8: s="N8";
			break;
	case (int)arithmeticExpr: s="arithmeticExpr";
			break;
	case (int)N4: s="N4";
			break;
	case (int)term_1: s="term_1";
			break;
	case (int)N5: s="N5";
			break;
	case (int)factor: s="factor";
			break;
	case (int)op1: s="op1";
			break;
	case (int)op2: s="op2";
			break;
	case (int)logicalOp: s="logicalOp";
			break;
	case (int)relationalOp: s="relationalOp";
			break;
	case (int)declareStmt: s="declareStmt";
			break;
	case (int)conditionalStmt: s="conditionalStmt";
			break;
	case (int)caseStmts: s="caseStmts";
			break;
	case (int)N9: s="N9";
			break;
	case (int)value: s="value";
			break;
	case (int)default_1: s="default_1";
			break;
	case (int)iterativeStmt: s="iterativeStmt";
			break;
	case (int)range: s="range";
			break;
	default: s="error";
}

	return s;

}

char * getLexeme(terminal t)
{
	char * s;
	switch ((int)t)
	{
	 case (int)INTEGER: s="integer";
			break;
	 case (int)REAL: s="real";
			break;
	 case (int)BOOLEAN: s="boolean";
			break;
	 case (int)OF: s="of";
			break;
	 case (int)ARRAY: s="array";
			break;
	 case (int)START: s="start";
			break;
	 case (int)END: s="end";
			break;
	 case (int)DECLARE: s="declare";
			break;
	 case (int)MODULE: s="module";
			break;
	 case (int)DRIVER: s="driver";
			break;
	 case (int)PROGRAM: s="program";
			break;
	 case (int)GET_VALUE: s="get_value";
			break;
	 case (int)PRINT: s="print";
			break;
	 case (int)USE: s="use";
			break;
	 case (int)WITH: s="with";
			break;
	 case (int)PARAMETERS: s="parameters";
			break;
	 case (int)TRUE_: s="true";
			break;
	 case (int)FALSE_: s="false";
			break;
	 case (int)TAKES: s="takes";
			break;
	 case (int)INPUT: s="input";
			break;
	 case (int)RETURNS: s="returns";
			break;
	 case (int)AND: s="AND";
			break;
	 case (int)OR: s="OR";
			break;
	 case (int)FOR: s="for";
			break;
	 case (int)IN: s="in";
			break;
	 case (int)SWITCH: s="switch";
			break;
	 case (int)CASE: s="case";
			break;
	 case (int)BREAK: s="break";
			break;
	 case (int)DEFAULT: s="default";
			break;
	 case (int)WHILE: s="while";
			break;
	 case (int)PLUS: s="+";
			break;
	 case (int)MINUS: s="-";
			break;
	 case (int)MUL: s="*";
			break;
	 case (int)DIV: s="/";
			break;
	 case (int)LT: s="<";
			break;
	 case (int)LE: s="<=";
			break;
	 case (int)GE: s=">";
			break;
	 case (int)GT: s=">=";
			break;
	 case (int)EQ: s="==";
			break;
	 case (int)NE: s="!=";
			break;
	 case (int)DEF: s="<<";
			break;
	 case (int)ENDDEF: s=">>";
			break;
	 case (int)COLON: s=":";
			break;
	 case (int)RANGEOP: s="...";
			break;
	 case (int)SEMICOL: s=";";
			break;
	 case (int)COMMA: s=",";
			break;
	 case (int)ASSIGNOP: s=":=";
			break;
	 case (int)SQBO: s="[";
			break;
	 case (int)SQBC: s="]";
			break;
	 case (int)BO: s="(";
			break;
	 case (int)BC: s=")";
			break;
	 case (int)COMMENTMARK: s="**";
			break;
	 case (int)DRIVERENDDEF: s=">>>";
			break;
	 case (int)DRIVERDEF: s="<<<";
			break;
	 case (int)$: s="$";
			break;
	 case (int)e: s="e";
			break;
	 case (int)null_point: s="null_point";
			break;
	 default : s="error";
	}
	return s;
}