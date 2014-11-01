//void assembly_generator(ASTree astTree,hashTable2 h_ID);
#ifndef assembler
#define assembler
#include "ASTops.h"

void generate_data(hashTable2 h_ID, FILE * fp);
void evaluateNode(ASTnode a, FILE * fp);
void evaluateQueue(ASTnode a,FILE * fp);
void evaluateNode(ASTnode a, FILE * fp);
void evaluateQueue(ASTnode a,FILE * fp);


#endif
