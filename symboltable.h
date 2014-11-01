#ifndef st
#define st
#include "hash2.h"
#include "lexer.h"
void createIDTable (tokenInfo tok,hashTable2 h_fn, hashTable2 h_ID, Total_Scope_List scope_tot, int print);
void second_run(tokenInfo tok,hashTable2 h_fn, hashTable2 h_ID,int print);
int check_module_return(int end_point, tokenInfo t, hashTable2 h_ID, IDEnt control_var);
void check_for(tokenInfo t, hashTable2 h_ID, IDEnt control_var, int print);
void print_ST(FILE *fp,hashTable2 h, Total_Scope_List t, int option);
char* type_get(Symbol s);
int assign_width(Symbol s);
#endif
