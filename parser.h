//parser.h
#ifndef parser
#define parser
#include "lexer.h"

#define GRAMCONST 95
int parse_status;
struct P_node;
struct ParseTreeQueue;
typedef struct P_node * pnode;
typedef struct ParseTreeQueue pTqueue;

typedef Symbol terminal;

typedef enum
{program, moduleDeclarations , moduleDeclaration , otherModules , driverModule , module , ret , input_plist , N1 , output_plist , N2 , dataType , type , moduleDef, statements , statement , ioStmt , var  , whichID , simpleStmt , assignmentStmt , whichStmt , lvalueIDStmt  , lvalueARRStmt  , index_nt  , moduleReuseStmt  , optional 
, idList , N3  , expression , arithmeticOrBooleanExpr , N7 , AnyTerm , N8  , arithmeticExpr , N4  , term_1 , N5  , factor , op1 , op2 , logicalOp , relationalOp  , declareStmt ,
conditionalStmt , caseStmts , N9 , value , default_1 , iterativeStmt , range} nonTerminal;
 
typedef union
{ terminal t;
  nonTerminal nt;
} G_element; //grammar element

typedef enum
{ term, non_term} tag;

struct ParseTreeQueue
{
pnode front;
pnode back;
int stacksize;
};

struct P_node
{
 pnode parent;
 pTqueue child;
 G_element e;
 tag t;
 int no_of_children;
 int level;
 tokenInfo tokenLink;
 pnode next;
};

struct ParseTree
{
pnode root;
pnode currentNode;
int height;
};

typedef struct ParseTree parseTree;

struct G_node
{
G_element ge;
tag t;
struct G_node * next;
};

typedef struct G_node * gnode;

struct G_head
{
nonTerminal t;
struct G_node * top;
};

typedef struct G_head gHead;
typedef gHead *Grammar;

struct StackForPARSE_TREE
{struct G_node* top;
 int size;
};

typedef  struct StackForPARSE_TREE parseTreeStack;
typedef int table[51][57];

struct FIRST_SET
{
terminal first[15];
};

typedef struct FIRST_SET first_Set;

first_Set firstSet[51];

struct FOLLOW_SET
{
terminal follow[15];
};

typedef struct FOLLOW_SET follow_Set;

follow_Set followSet[51];


pTqueue initializeQueue();

pTqueue enqueue(pnode q,pnode p);

Grammar createGrammar();

struct G_node * createGrammarNode(G_element ge, tag t);

#endif

void createFirstSets();

void createFollowSets();

void fillfollow(nonTerminal nt, int ind,Grammar G);

void fillForFirst(gnode g,int ind,Grammar G);

void createParseTable(Grammar G);

void printParseTable();

void initializeParseTable();

char * getToken(terminal t);

char * getLexeme(terminal t);

parseTree parseInputSourceCode(char * testcaseFile, hashTable h, Grammar G, int print);

pnode createPnodeNonTerminal(pnode Parent,nonTerminal nt,tag t,int no_of_children, int level);

parseTreeStack mergeStack(gHead g1,parseTreeStack s);	

parseTreeStack pop(parseTreeStack s);

pnode createPnodeTerminal(pnode Parent,terminal term_1,tag t,int level,tokenInfo token);

void printParseTree( char * outfile, parseTree PT);

char * getNON_term(nonTerminal nt);

void printNode(FILE * fp, pnode N);

void printParseTreeQueue(FILE *fp,pnode  p);

pnode returnCurrent(pnode P);
