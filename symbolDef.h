#ifndef data3
#define data3

#include "lexer.h"

int semantic_status;
typedef char IDname[10];

typedef enum
{ defined, declared} fnstatus;

struct idEnt;
typedef struct idEnt* IDEnt;

typedef struct 
{
IDEnt node;
int code;
}findresult2;

struct array
{
	int range_s;
	int range_f;
	Symbol type;
}; 

struct variable
{
	Symbol type;
};

struct id_variable
{
	IDname word;
	int lno;
	int code;	
	int v_or_a;
	int scope;
	int w;
	union
	{
		struct variable v;
		struct array a;
	} var;
};



struct parameter;
typedef struct parameter* Parameter;

struct scope_chain;
typedef struct scope_chain* Scope_Chain;

struct scope_chain
{
	int scope;
	int in_switch;
	int in_loop;
	Scope_Chain next;
	Scope_Chain prev;
};

struct total_scope_list;
typedef struct total_scope_list* Total_Scope_List;

struct total_scope_list
{
	int scope_start;
	int scope_end;
};
	
	

struct parameter 
{
	union
	{
		struct array a;
		struct variable v;
	} var;
	int v_or_a;
	Parameter next;
};

struct id_function
{
	IDname word;
	int scope;
	int lno;
	int used;
	fnstatus status;
	Parameter i_start;
	Parameter o_start;
};
	
	
struct idEnt
{
	union
	{
		struct id_variable ivar;
		struct id_function ifn;
	} entity;
};//ID structure
#endif
