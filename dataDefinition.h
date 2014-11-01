#ifndef data
#define data

#define x 5
#define keycount 30
typedef int size;

typedef char keyword[15];
	
typedef enum{available, occupied, deleted}status;

struct _chain;
typedef struct _chain *chain;

typedef struct 
{
	keyword k;
	int code;
}findresult;


typedef struct
{
	int code;	
	keyword word;
	status flag;
	chain start;
}cell;

struct _chain
{
	int code;
	keyword word;
	chain next;
};

typedef cell *hashTable;
#endif

hashTable initializeHashTable(size l);
int hashValue(char *key);
findresult find(hashTable T, char *searchKey);
void addKeyword(hashTable T, int c, keyword K);

