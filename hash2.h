#ifndef data2
#define data2
#include "lexer.h"
#include"symbolDef.h"
struct _chain2;
typedef struct _chain2 *chain2;



typedef struct
{
	status flag;
	IDEnt ID;
	chain2 start;
}cell2;

struct _chain2
{
	IDEnt ID;
	chain2 next;
};

typedef cell2 *hashTable2;


#endif

hashTable2 initializeHashTable2(size l);
int hashValue2(char *key);
findresult2 find2(hashTable2 T, char *searchKey, int scope, int option);
void addKeyword2(hashTable2 T, IDEnt ID,int option);

