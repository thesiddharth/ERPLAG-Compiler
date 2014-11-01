#ifndef lexer
#define lexer
#include <stdio.h>
#include "dataDefinition.h"
#include <stdlib.h>



typedef enum 
{
INTEGER,REAL,BOOLEAN,OF,ARRAY,START,END,DECLARE,MODULE,DRIVER,PROGRAM,GET_VALUE,PRINT,USE,WITH,PARAMETERS,TRUE_,FALSE_,TAKES,INPUT,RETURNS,AND,OR,FOR,IN,SWITCH,CASE,BREAK,DEFAULT,WHILE,PLUS,MINUS,MUL,DIV,LT,LE,GE,GT,EQ,NE,DRIVERDEF,DRIVERENDDEF,DEF,ENDDEF,COLON,RANGEOP,SEMICOL,COMMA,ASSIGNOP,SQBO,SQBC,BO,BC,COMMENTMARK,ID,NUM,RNUM, $, e, null_point=-1
}Symbol;//Terminal enumeration

struct SymEnt;
typedef struct SymEnt *tokenInfo;
 
struct SymEnt
{
	Symbol s;
	union
	{
		int i;
		float f;
		char s[10];
	} val;
	int lno;
	int df;
	tokenInfo n;
	tokenInfo prev;
};//Symbol structure


#endif

void addRNum(float num,int rnum_no);
void addNum(float num,int num_no);
void addID(char * id);
void addSym (int n);
void matcher(hashTable h, char *buf, int w);
void removeComments(char *testcaseFile, char *cleanFile);
char* getStream();
tokenInfo getNextToken();
void populateSymbolTable(char *opfile, hashTable h, int f_or_c);
tokenInfo getFirstToken();
