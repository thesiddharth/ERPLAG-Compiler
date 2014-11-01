#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash2.h"

hashTable2 initializeHashTable2(size l)
{
    hashTable2 ht;
    ht = (hashTable2)malloc(l*sizeof(cell2));
    int i=0;
    for(i=0;i<l;i++)
    {
        ht[i].flag=available;
	ht[i].start=NULL;
	ht[i].ID=NULL;
    }
return(ht);
}

int hashValue2(char *key)
{
	int ref=(int)'0';    
	int l = strlen(key);    
        int h=((int)key[l-1]-ref),p,i;
	for(i=l-2;i>=0;i--)
    	{
        	h=(int)((h*x)+((int)key[i]-ref));
	}
    p = h%13;
    return(p);
}

findresult2 find2(hashTable2 T, char *searchKey, int scope,int option)
{	
	int index = hashValue2(searchKey);
    	findresult2 r;	
	if(option==0)
	{	
		if(T[index].flag==available)
    		{
			r.code=-1;
              	 	return r;
      		}    
     		else if(strcmp(T[index].ID->entity.ivar.word,searchKey)!=0)
  		{
			chain2 temp = (chain2)malloc(sizeof(struct _chain2));
			temp=T[index].start;
			while (temp!=NULL)
			{
				if(strcmp(temp->ID->entity.ivar.word ,searchKey)==0)
				{	
					if(scope==temp->ID->entity.ivar.scope)
					{	
						r.node=(temp->ID);
						r.code=1;				
						return r;
					}
				temp=temp->next;
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
			if(scope==T[index].ID->entity.ivar.scope)
			{
				r.node=(T[index].ID);
				r.code=1;				
				return r;
			}
			else
			{
				chain2 temp = (chain2)malloc(sizeof(struct _chain2));
				temp=T[index].start;
				while (temp!=NULL)
				{
					if(strcmp(temp->ID->entity.ivar.word ,searchKey)==0)
					{	
						if(scope==temp->ID->entity.ivar.scope)
						{	
							r.node=(temp->ID);
							r.code=1;				
							return r;
						}
						
						temp=temp->next;
					}           
		       			else
					{
						temp=temp->next;
					}
				}
				r.code=-1;
				return r;
			} 
		}
	}
	else
	{
		if(T[index].flag==available)
    		{
			r.code=-1;
              	 	return r;
      		}    
     		else if(strcmp(T[index].ID->entity.ifn.word,searchKey)!=0)
  		{
			chain2 temp = (chain2)malloc(sizeof(struct _chain2));
			temp=T[index].start;
			while (temp!=NULL)
			{
				if(strcmp(temp->ID->entity.ifn.word ,searchKey)==0)
				{	
					r.node=(temp->ID);
					r.code=1;				
					return r;
				}
				temp=temp->next;
			}
			r.code=-1;
			return r;
		}
		else
		{
			r.node=(T[index].ID);
			r.code=1;				
			return r;
		}
	}
}
    


void addKeyword2(hashTable2 T, IDEnt t, int option)
{
	int index;
	if(option==0)   
	{	index = hashValue2(t->entity.ivar.word);}
	else
	{	index = hashValue2(t->entity.ifn.word);}
        if(T[index].flag==available||T[index].flag==deleted)
        {
		T[index].flag=occupied;
		T[index].start=NULL;
		T[index].ID=t;
	}
        else 
	{
		chain2 temp = (chain2)malloc(sizeof(struct _chain2));
		temp->ID=t;   
   		temp->next=NULL;
		chain2 parent = T[index].start;		
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
