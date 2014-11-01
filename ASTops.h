//ASTops.h
#ifndef ASTops
#define ASTops
#include "ASTDef.h"


int rem_PT(terminal t);// check whether to remove from PARSE Tree to get the AST
ASTnode createASTNode(ASTnode parent,  G_element e,  tag t, tokenInfo tokenLink, IDEnt link,  valueOfASTnode val,  terminal tp, ASTnode prev, FILE * fp);
ASTQueue createASTQueue();
ParentQ createPQueue();
PQnode createPQnode(nonTerminal nt);
ParentQ enPQueue(ParentQ q,nonTerminal t);
void printPQueue();
int isaop(terminal t);
int islop(terminal t);
int isrelop(terminal t);
ASTree buildASTree(parseTree PT,hashTable2 h_ID, hashTable2 h_fn, char * outputfile);
ASTnode FillQueue(pnode p,ASTnode a,ASTnode parent,ASTnode prev, hashTable2 h_ID, hashTable2 h_fn, FILE * fp);
struct ASTQUEUE enASTqueue(struct ASTQUEUE q,ASTnode p);
ASTnode FillNodeAST(pnode p,ASTnode parent,ASTnode prev, hashTable2 h_ID, hashTable2 h_fn, FILE * fp);
ASTnode typeAssignerAndChecker(ASTnode a, int ToPrint);
void printASTree(ASTree a, FILE * fp);
void printASTreeQueue(FILE * fp, ASTnode a);
void printASTnode(FILE *fp, ASTnode a);
int ASTQueueTypeChecker(ASTnode a, int ToPrint);

#endif

