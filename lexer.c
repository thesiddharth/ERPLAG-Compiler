#include "lexer.h"
#include "dataDefinition.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

int flag=0;	
int line_num=1;
int id_num=1;
tokenInfo end;
tokenInfo first;
tokenInfo scan;
FILE *fp;

static FILE* f_read;

int Pow(int power,int n)
{
if (power==0)return 1;
else return (n*Pow(power-1,n));
}


void matcher(hashTable h, char *buf, int w)//Matches line with correct token
{
	int index=0;
	int l = strlen(buf);	
	char lookahead = buf[0];
	static int num_no=1;
	static int rnum_no=1;	
		
	while(index < l)
	{
		if (flag==1)
		{	
			while(!((buf[index]=='*')&&(buf[index+1]=='*'))&&(index < l))
			{
				index++;
			}
			if(index==l)
			{
				index--;
			}
		lookahead=buf[index];
		}

		if ((lookahead>='A'&&lookahead<='Z')||(lookahead>='a'&&lookahead<='z'))//ID or keyword is checked through hashing
		{
			char id[20]="";int i=0;findresult r;
			id[i]=lookahead;			
			index ++;			
			while (((buf[index]>='A'&&buf[index]<='Z')||(buf[index]>='a'&&buf[index]<='z')||(buf[index]=='_')||(buf[index]>='0'&&buf[index]<='9'))&&buf[index]!='\n')
			{
				id[++i]=buf[index++];
				
			}
			r = find(h, id);
			if (r.code == -1)
			{
				if (strlen(id)<=8) 				
				{				
					addID(id);
					fprintf(fp," ID");
				}
				else
				{
					if(w==1)
						printf("\nERROR 1 :Identifier %s at line %d is longer than 8 characters",id,line_num);
					fprintf(fp," _!!_");
				}
			}
			else
			{
				int j;fprintf(fp," ");
				for(j=0;j<strlen(r.k);j++)				
				fprintf(fp,"%c",toupper(r.k[j]));
				addSym(r.code);
			}
		index--;
		}
		else if (lookahead>='0'&&lookahead<='9')//NUM or RNUM
		{
			int ref=(int)'0';
			int num1=0, num2=0;
			int dec =0;
			int power=0;			
			num1=((int)buf[index])-ref;
			while(buf[index+1]>='0'&&buf[index+1]<='9')
			{
				num1=(num1*10)+(((int)buf[index+1])-ref);	
				index++;
			}
			if(buf[index+1]=='.')
			{
				if(buf[index+2]!='.')
				{	
					index++;
					if(buf[index+1]<'0'||buf[index+1]>='9')
					{
						if(w==1)
							printf("\nInvalid real number at line %d, column %d : No digit after floating point ",line_num,(index+1));
						fprintf(fp," _!!_");
						index++;
					}
					while(buf[index+1]>='0'&&buf[index+1]<='9')
					{
						index++;						
						num2=(num2*10)+(((int)buf[index])-ref);
						dec++;					
					}
					if(buf[index+1]=='e'||buf[index+1]=='E')
					{
						index+=2;
						if(buf[index]=='+')
						{
							while(buf[index+1]>='0'&&buf[index+1]<='9')
							{
								index++;							
								power=(power*10)+(((int)buf[index])-ref);
							}
						}
						else if(buf[index]=='-')
						{
							while(buf[index+1]>='0'&&buf[index+1]<='9')
							{
								index++;							
								power=(power*10)+(((int)buf[index])-ref);
							}
						power=-(power);						
						}
						else if(buf[index]>='0'&&buf[index]<='9')
						{
							while(buf[index]>='0'&&buf[index]<='9')
							{
								power=(power*10)+(((int)buf[index])-ref);
								index++;
							}
						index--;			
						}
						else
						{
							if(w==1)
								fprintf(fp,"\nInvalid real number at line %d, column %d : invalid character after exponent symbol",line_num, index);
							fprintf(fp," _!!_");
						}
					}					
					else
						power=0;		
				
					float num = (float)num1 + (float)(num2/(Pow(10,dec)));
					num*=Pow(10,power);
					fprintf(fp," RNUM%d",rnum_no++);
					addRNum(num,rnum_no);
				}
				else
				{
					fprintf(fp," NUM%d",num_no++);
					addNum(num1,num_no);	
				}
			}
			else
			{
				fprintf(fp," NUM%d",num_no++);
				addNum(num1,num_no);	
			}
		}			
		else if (lookahead=='+')
		{
			fprintf(fp," PLUS");			
			addSym(40);
		}			
		else if (lookahead=='-')
		{
			fprintf(fp," MINUS");			
			addSym(41);
		}
		else if (lookahead=='*')
		{
			if(buf[index+1]=='*')
			{			
				fprintf(fp," COMMENTMARK");			
				addSym(63);
				index+=2;
				if(flag==1) {flag=0;lookahead=buf[index];continue;}
				else flag=1;
				while(!((buf[index]=='*')&&(buf[index+1]=='*'))&&(index < l))
				{
					index++;
				}
				index--;
			}
			else
			{
				fprintf(fp," MUL");			
				addSym(42);
			}
		}
  		else if (lookahead=='/')
		{
			fprintf(fp," DIV");			
			addSym(43);
		}
		else if (lookahead=='<')
		{
			if(buf[index+1] == '=')
			{
				fprintf(fp," LE");			
				addSym(45);	
				index++;			
			}
			else if(buf[index+1] == '<')
			{
				if(buf[index+2]=='<')
				{
					fprintf(fp," DRIVERDEF");			
					addSym(46);
					index+=2;
				}
				else
				{
					fprintf(fp," DEF");			
					addSym(47);
					index++;
				}
			}
			else
			{
				fprintf(fp," LT");			
				addSym(44);			
			}
		}
		else if (lookahead=='>')
		{
			if(buf[index+1] == '=')
			{
				fprintf(fp," GE");			
				addSym(49);	
				index++;			
			}
			else if(buf[index+1] == '>')
			{
				if(buf[index+2]=='>')
				{
					fprintf(fp," DRIVERENDDEF");			
					addSym(50);
					index+=2;
				}
				else
				{
					fprintf(fp," ENDDEF");			
					addSym(51);
					index++;
				}
			}
			else
			{
				fprintf(fp," GT");			
				addSym(48);			
			}
		}
		else if (lookahead=='=')
		{
			if(buf[index+1]=='=')
			{			
				fprintf(fp," EQ");			
				addSym(52);
				index++;
			}
			else
			{
				if(w==1)
					printf("\nInvalid symbol '=' at line %d, column %d",line_num, index);
				fprintf(fp," _!!_");
				index++;
			}
		}
		else if (lookahead=='!')
		{
			if(buf[index+1]=='=')
			{			
				fprintf(fp," NE");			
				addSym(53);
				index++;
			}
			else
			{
				if(w==1)
					printf("\nInvalid symbol '!' at line %d, column %d",line_num, index);
			}
		}
		else if (lookahead==':')
		{
			if(buf[index+1]=='=')
			{			
				fprintf(fp," ASSIGNOP");			
				addSym(54);
				index++;
			}
			else
			{
				fprintf(fp," COLON");			
				addSym(55);
			}
		}
		else if (lookahead=='.')
		{
			if(buf[index+1]=='.')
			{			
				fprintf(fp," RANGEOP");			
				addSym(56);
				index++;
			}
			else
			{
				if(w==1)
					printf("\nInvalid symbol '.' at line %d, column %d",line_num, index);
			}
		}
		else if (lookahead==';')
		{
			fprintf(fp," SEMICOL");			
			addSym(57);
		}
		else if (lookahead==',')
		{
			fprintf(fp," COMMA");			
			addSym(58);
		}
		else if (lookahead=='[')
		{
			fprintf(fp," SQBO");			
			addSym(59);
		}
		else if (lookahead==']')
		{
			fprintf(fp," SQBC");			
			addSym(60);
		}		
		else if (lookahead=='(')
		{
			fprintf(fp," BO");			
			addSym(61);
		}	
		else if (lookahead==')')
		{
			fprintf(fp," BC");			
			addSym(62);
		}	
		else if(buf[index]=='\n')
		{
			fprintf(fp,"\n");
			line_num++;	
		}	
		else if(buf[index]!=' '&&buf[index]!='\t')
		{
			if(w==1)
				printf("\nInvalid symbol %c at line %d, column %d",buf[index],line_num, index);
			fprintf(fp," _!!_");
		}
		index++;
		while(buf[index]==' '||buf[index]=='\t')
			index++;
		lookahead=buf[index];
	}
}

void addSym (int n)//Add a symbol
{
	tokenInfo temp=(tokenInfo)malloc(sizeof(struct SymEnt));
	switch(n)
	{
		case 0: temp->s=INTEGER;
			break;
		case 1: temp->s=REAL;
			break;
		case 2: temp->s=BOOLEAN;
			break;
		case 3: temp->s=OF;
			break;
		case 4: temp->s=ARRAY;
			break;
		case 5: temp->s=START;
			break;
		case 6: temp->s=END;
			break;
		case 7: temp->s=DECLARE;
			break;
		case 8: temp->s=MODULE;
			break;
		case 9: temp->s=DRIVER;
			break;
		case 10:temp->s=PROGRAM;
			break;
		case 11:temp->s=GET_VALUE;
			break;
		case 12:temp->s=PRINT;
			break;
		case 13:temp->s=USE;
			break;
		case 14:temp->s=WITH;
			break;
		case 15:temp->s=PARAMETERS;
			break;
		case 16:temp->s=TRUE_;
			break;
		case 17:temp->s=FALSE_;
			break;
		case 18:temp->s=TAKES;
			break;
		case 19:temp->s=INPUT;
			break;
		case 20:temp->s=RETURNS;
			break;
		case 21:temp->s=AND;
			break;
		case 22:temp->s=OR;
			break;
		case 23:temp->s=FOR;
			break;
		case 24:temp->s=IN;
			break;
		case 25:temp->s=SWITCH;
			break;
		case 26:temp->s=CASE;
			break;
		case 27:temp->s=BREAK;
			break;
		case 28:temp->s=DEFAULT;
			break;
		case 29:temp->s=WHILE;
			break;
		case 40:temp->s=PLUS;
			break;
		case 41:temp->s=MINUS;
			break;
		case 42:temp->s=MUL;
			break;
		case 43:temp->s=DIV;
			break;
		case 44:temp->s=LT;
			break;
		case 45:temp->s=LE;
			break;
		case 46:temp->s=DRIVERDEF;
			break;
		case 47:temp->s=DEF;	
			break;
		case 48:temp->s=GT;
			break;
		case 49:temp->s=GE;
			break;
		case 50:temp->s=DRIVERENDDEF;
			break;
		case 51:temp->s=ENDDEF;
			break;
		case 52:temp->s=EQ;
			break;
		case 53:temp->s=NE;
			break;
		case 54:temp->s=ASSIGNOP;
			break;
		case 55:temp->s=COLON;
			break;
		case 56:temp->s=RANGEOP;
			break;
		case 57:temp->s=SEMICOL;
			break;
		case 58:temp->s=COMMA;
			break;
		case 59:temp->s=SQBO;
			break;
		case 60:temp->s=SQBC;
			break;
		case 61:temp->s=BO;
			break;
		case 62:temp->s=BC;	
			break;
		case 63:temp->s=COMMENTMARK;
			break;
		default: {
			 FILE *fp;
			 fp=fopen("error.txt","a");
			 fprintf(fp,"\nWrong symbol number %d passed to addSym",n);
			 fclose(fp);
			 }
	}
	temp->n=NULL;
	temp->prev=end;
	temp->lno=line_num;
	end->n=temp;
	end=end->n;
}

void addID(char * id)//Add an ID
{
	tokenInfo temp=(tokenInfo)malloc(sizeof(struct SymEnt));
	temp->s=ID;	
	strcpy(temp->val.s,id);
	temp->lno=line_num;
	temp->n=NULL;
	temp->prev=end;
	end->n=temp;
	end=end->n;
}	

void addRNum(float num,int rnum_no)//Add a floating point number
{
	tokenInfo temp=(tokenInfo)malloc(sizeof(struct SymEnt));
	temp->s=RNUM;
	temp->val.f=num;
	temp->df=rnum_no;
	temp->lno=line_num;
	temp->n=NULL;
	temp->prev=end;
	end->n=temp;
	end=end->n;
}
	
void addNum(float num,int num_no)//Add an integer
{
	tokenInfo temp=(tokenInfo)malloc(sizeof(struct SymEnt));
	temp->s=NUM;	
	temp->val.i=num;
	temp->df=num_no;
	temp->lno=line_num;
	temp->n=NULL;
	temp->prev=end;
	end->n=temp;
	end=end->n;
}

char *getStream()//Get line from input file into buffer
{
	char *try=(char *)malloc(200*sizeof(char));
	fgets(try,200,f_read);
	if(feof(f_read))
		return "";
	if (strlen(try)==200)
	{
		int index=199;
		while(!(try[index]==' '||try[index]=='\t'))
		{
			index--;
		}
		try[index+1]='\0';
		fseek(f_read,-(199-index),SEEK_CUR);
	}	
	return try;
}

tokenInfo getNextToken()//Get the next  token in symbol table
{
	if(scan!=NULL)
	{
		scan=scan->n;	
		return(scan);
	}
	else
	{
		scan=first;
		return(scan);
	}
} 

tokenInfo getFirstToken()//Get the next  token in symbol table
{
	return first->n;
} 

void populateSymbolTable(char *opfile, hashTable h, int p_or_c)
{
	f_read=fopen(opfile,"r");	
	fp = fopen("tokenstream.txt","w");
	first = (tokenInfo)malloc(sizeof(struct SymEnt));
	end = (tokenInfo)malloc(sizeof(struct SymEnt));
	first->df=-1;
	end = first;
	char buf[200];
	while(1)
	{	
		strcpy(buf,getStream());	
		if(strcmp(buf,"")!=0)
			matcher(h,buf, p_or_c);
		else
			break;
	}
	scan=first;
	fclose(fp);
	flag=0;	
	line_num=1;
	id_num=1;
}
		
void removeComments(char *testcaseFile, char *cleanFile)
{
	FILE *f1;
	f_read=fopen(testcaseFile,"r");
	f1=fopen(cleanFile,"w");
	char buf[200]="";
	while(1)
	{
		strcpy(buf,getStream());	
		if(strcmp(buf,"")!=0)
		{
			int index=0;
			int l = strlen(buf);
			while(index < l)
			{
				if (flag==1)
				{	
					while(!((buf[index]=='*')&&(buf[index+1]=='*'))&&(index < l))
					{
						index++;
					}
				}
				while(!((buf[index]=='*')&&(buf[index+1]=='*'))&&(index < l))
				{
					fprintf(f1,"%c",buf[index]);
					index++;
				}			
				if(index!=l)
				{
					index+=2;
					if(flag==1) 
					{
						flag=0;
						continue;
					}
					else 
					{
						flag=1;
						continue;
					}
				}
			}
		}
		else
			break;
	}
	fclose(f1);
}
