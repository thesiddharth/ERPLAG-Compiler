// ASTDef.h
#ifndef ASTDEF
#define ASTDEF
#include"parser.h"
#include "hash2.h"


typedef struct AST_NODE * ASTnode;
typedef struct AStree AST;
typedef struct bTnode opNode;
typedef struct ASTQUEUE ASTQueue;
typedef struct AST_Tree ASTree;
typedef struct PQNode * PQnode;
typedef struct P_Queue ParentQ;
//int ERROR_IN_TYPE;

typedef enum
{Tr=1,Fls=0}boolean;

typedef union
{
int num;
float rnum;
boolean t_val;
char na;
}valueOfASTnode;

struct ASTQUEUE 
{
ASTnode front;
ASTnode back;
int size;
};

struct P_Queue
{
PQnode front;
PQnode back;
int size;
};

struct PQNode
{
nonTerminal nt;
PQnode next;
};


struct AST_NODE
{ ASTnode parent;
  struct ASTQUEUE childQ;
  G_element e;
  ParentQ plist;
  tag t;
  tokenInfo tokenLink;
  IDEnt link; //Symbol Table Index
  valueOfASTnode val;
  Symbol tp;//type
  ASTnode next;
  ASTnode prev;
  int scope;
};


struct AST_Tree
{
ASTnode root;
ASTnode currentNode;
};


#endif

   
