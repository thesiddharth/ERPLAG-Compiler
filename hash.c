#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataDefinition.h"

hashTable initializeHashTable(size l)
{
    hashTable ht;
    ht = (hashTable)malloc(l*sizeof(cell));
    int i=0;
    for(i=0;i<l;i++)
    {
        ht[i].flag=available;
	ht[i].start=(chain)malloc(sizeof(struct _chain));
    }
return(ht);
}

int hashValue(char *key)
{
	int ref=(int)'0';    
	int l = strlen(key);    
        int h=((int)key[l-1]-ref),p,i;
	for(i=l-2;i>=0;i--)
    	{
        	h=(int)((h*x)+((int)key[i]-ref));
	}
    p = h%keycount;
    return(p);
}

findresult find(hashTable T, char *searchKey)
{
    	int index = hashValue(searchKey);
    	findresult r;
    	if(T[index].flag==available)
    	{
		r.code=-1;
                return r;
      	}    
     	else if(strcmp(T[index].word,searchKey)!=0)
  	{
		chain temp = (chain)malloc(sizeof(struct _chain));
		temp=T[index].start;
		while (temp!=NULL)
		{
			if(strcmp(temp->word ,searchKey)==0)
			{	
				strcpy(r.k,searchKey);
				r.code=temp->code;				
				return r;    
    			}           
		        else
			{
				temp=temp->next;
			}
		}
		r.code=-1;
		return r;
	}
	else
	{
		strcpy(r.k,searchKey);
		r.code=T[index].code;
		return r;
	}
}    
    


void addKeyword(hashTable T, int c, keyword K)
{   
	int index = hashValue(K);
        if(T[index].flag==available||T[index].flag==deleted)
        {
	        strcpy(T[index].word,K);
   		T[index].code=c;    
   		T[index].flag=occupied;
		T[index].start=NULL;
	}
        else 
	{
		chain temp = (chain)malloc(sizeof(struct _chain));
		strcpy(temp->word,K);
   		temp->code=c;    
   		temp->next=NULL;
		chain parent = T[index].start;		
		if(parent!=NULL)		
		{
			while (parent->next!=NULL)
			{	
				parent=parent->next;
			}
			parent->next=temp;	
		}
		else
		{
			T[index].start=temp;
		}
	}
}
