#include<iostream>
#include<stdio.h>
#include<fstream>
#include<cstdlib>
#include<string>
#include<cstring>
#include<sstream>
#include<cmath>
#include"test.cpp"
using namespace std;

/*-------------------------------------------------------------------------------------
	To store lines from the input file 
-------------------------------------------------------------------------------------*/
struct line
{
	char label[10];
	char operand[10];
	instruction mnemonic;
	unsigned int obcode;
	unsigned int address;		
};

/*------------------------------------------------------------------------------------------
	To store the symbols/labels used in the program.
------------------------------------------------------------------------------------------*/
struct symbol
{
	char label[10];
	unsigned int address;
};

/*------------------------------------------------------------------------------------------
	TO PERFORM LINEAR SEARCH IN THE OPTAB
------------------------------------------------------------------------------------------*/
int linearsearch(instruction ob[], const char key[])
{
	int i=-1;
	while (i<64)
	{
		if(strcmp(key,ob[++i].name)==0)
		break;
	}
	return i;
}
/*-----------------------------------------------------------------------------------------
	TO CONVERT STRING TO HEX
-----------------------------------------------------------------------------------------*/
int toHex(string str)
{	
	stringstream s(str);
	int value;
	s>>hex>>value;
	return value;
}
/*-----------------------------------------------------------------------------------------
	TO CONVERT STRING TO DECIMAL
-----------------------------------------------------------------------------------------*/
int toDec(string str)
{	
	stringstream s(str);
	int value;
	s>>value;
	return value;
}
/*-----------------------------------------------------------------------------------------
	TO CONVERT DECIMAL TO STRING 
-----------------------------------------------------------------------------------------*/
string toString(int val)
{
	string result;
	stringstream convert;
	convert<<val;
	result=convert.str();
	return result;
}
/*-----------------------------------------------------------------------------------------
	TO DISPLAY THE SYMTAB CREATED IN PASS_1
-----------------------------------------------------------------------------------------*/
void symtab()
{
	ifstream search;
	search.open("sym_tab.bin",ios::binary|ios::in);
	symbol s1;	
	while(search)
	{
		search.read((char *)&s1,sizeof(s1));			
		cout<<s1.label<<"\t";
		printf("%.6X\n",s1.address);				
	}
	search.close();			
}
/*---------------------------------------------------------------------------------
	TO PERFORM PASS 1 AND CRAETE SYMTAB 
---------------------------------------------------------------------------------*/
unsigned int PASS_1()
{
	char ch;
	int LOCCTR=0x00;
	ifstream in;
	in.open("sample.txt",ios::in);
	ofstream infl;
	infl.open("program.bin",ios::binary|ios::out);
	ofstream sym;
	sym.open("sym_tab.bin",ios::binary|ios::out  );	
	sym.close();	
	while(in.get(ch))
	{
		
		string table[5];
		if(ch!='.')
		{	
			int fmt=0;
			unsigned long obc=0x0;
			table[0]=toString(LOCCTR);
			if(ch!=' '&&ch!='\t')
			{
				while(ch!=' '&& ch!='\t')
				{
					table[1]+=ch;
					in.get(ch);
				}
				ifstream search;
				search.open("sym_tab.bin",ios::binary|ios::in);
				while(search)
				{
					symbol s1;
					search.read((char *)&s1,sizeof(s1));			
					if(strcmp(table[1].c_str(),s1.label)==0)
					{
					cout<<"duplicate symbol";		
					return 0;
					}
				}
				search.close();
				ofstream sym;
				sym.open("sym_tab.bin",ios::binary|ios::out|ios::app);
				symbol s2;
				strcpy(s2.label,table[1].c_str());
				s2.address=LOCCTR;
				sym.write((char *)&s2,sizeof(struct symbol));
				sym.close();
			}
			else table[1]="";
			
			while(ch==' '|| ch=='\t')
			in.get(ch);
//		searching OPTAB for opcode

			while(ch!=' ' && ch!='\n' && ch!='\t')
			{
				if(ch=='+') 
				{
					fmt+=1;
					in.get(ch);
					continue;
				}
				table[2]+=ch;
				in.get(ch);
			}

			const char* symbol =table[2].c_str();
			int key=linearsearch(OPTAB,symbol);
			if(key<58)
			{
				obc=OPTAB[key].opcode;
				fmt+=OPTAB[key].format;
				LOCCTR+=fmt;
				obc=obc*pow(16,2*(fmt-1));	
			}
			
			while(ch==' '|| ch=='\t')
			in.get(ch);
			
			int flag=0;
			while(ch!='\n' && in)
			{
				while(flag==0 && key <58)
				if (ch=='#') 
				{
					if(fmt==3) obc+=0x010000;
					else obc+=0x01100000;
					in.get(ch);
					flag=1;
					continue;
				}
				else if(ch=='@')
				{
					if(fmt==3) obc+=0x020000;
					else obc+=0x02100000;
					in.get(ch);
					flag=1;
					continue;
				}
				else 
				{
					if(fmt==3) obc+=0x030000;
					else if(fmt==4) obc+=0x03100000;
					flag=1;
				}
				if(ch==',')
				{
					if(fmt==3) obc+=0x008000;
					else if(fmt==4) obc+=0x00900000;
					in.get(ch);
					if(ch=='X')
					{
						in.get(ch);
						break;
					}
					table[3]+=',';
				}
				table[3]+=ch;
				in.get(ch);
			}

			if(key>57)
			{
			
				if(table[2]=="WORD")
				{
					fmt=3;
					LOCCTR+=fmt;
					obc=toDec(table[3]);
				}
				else if(table[2]=="RESW") 
				LOCCTR+=(3*toDec(table[3]));
				else if(table[2]=="RESB") 
				LOCCTR+=toDec(table[3]);
				else if(table[2]=="BYTE")
				{
					int len;
					int i=2;
					char chr;
					string temp;
					chr=table[3][i];
					while(chr!='\'')
					{	
						temp+=chr;
						chr=table[3][++i];
					}
					len=i-2;
					fmt=len;			
					if(table[3][0]=='C')
					for(int i=0;i<len;i++)
					{
						obc=obc+temp[i]*pow(256,len-i-1);
					}
					else
					{
						obc=toHex(temp);
						fmt=len/2;
					}
					LOCCTR+=fmt;
		
				}
				else if(table[2]=="END")
				{
					fmt=0;
					table[0]="";
				}
				else if(table[2]=="BASE")
				{
					
				}
				else if(table[2]=="START")
				{
					fmt=0;
					LOCCTR=toHex(table[3]);
					table[0]=toString(LOCCTR);
				}
				else 
				{
				 	cout<<"invalid opcode!!";
				 	return 0;
				}
			}
			if(fmt>0) 
			{
				table[4]=toString(obc);

			}
			line ob;
			ob.address=toDec(table[0]);
			strcpy(ob.label,table[1].c_str());
			ob.mnemonic=OPTAB[key];
			strcpy(ob.operand,table[3].c_str());
			ob.obcode=toDec(table[4]);
			infl.write((char *)&ob,sizeof(struct line));
		}
		else while(ch!='\n') in.get(ch);
	}
	infl.close();
	in.close();
	return LOCCTR;
}

/*--------------------------------------------------------------------------------------------
	To search for a given operand in the SYMTAB defined in PASS_1 
	and return the address of the label. 
--------------------------------------------------------------------------------------------*/
unsigned int search(string cur_label,int index)
{
	ifstream sym;
    	sym.open("sym_tab.bin",ios::binary|ios::in);
	int found=0;
	unsigned int constant;
	
	if(!sym)
	{
		cout<<"\nTHE SYMTAB FILE IS MISSING\n";
		exit(0);
	}
	symbol temp;	
	
	while(sym)
	{	
		sym.read((char *)&temp,sizeof(temp));
		if(strcmp(temp.label,cur_label.c_str())==0)
		{
			found=1;
			break;
		}						
	}
	
	sym.close();
	if(found==1)
	{
		return temp.address;
	}	
//	if the given operand is not a symbol defined
	constant=toDec(cur_label);		
	if(constant!=0)
		return constant;
	else if(constant==0 && cur_label[0]=='0')
		return 0;
	else
	{
		cout<<"\n UNDEFINED LABEL USED IN LINE NO. :"<<index<<"\n";
		exit(0);
	}
		
}

/*--------------------------------------------------------------------------
	To return the hexadecimal equivalent of the 
	registers used in Format 2 instructions
---------------------------------------------------------------------------*/

unsigned int registers(string cur_label,int index)
{
	unsigned int reg=0x00;
	char ch=cur_label[0];
	
	switch(ch)
	{
		case 'A':reg=0x00;break;
		case 'X':reg=0x10;break;
		case 'L':reg=0x20;break;
		case 'B':reg=0x30;break;
		case 'S':reg=0x40;break;
		case 'T':reg=0x50;break;
		case 'F':reg=0x60;break;
		default :cout<<"\nINVALID REGISTER USE IN LINE NO. :"<<index;
			 exit(0);
	}
	
	ch=cur_label[1];
	if(ch=='\0'|| ch==' ')
		return reg;
//	if there are 2 registers
	if(ch==',')
	{
		ch=cur_label[2];
		switch(ch)
		{
			case 'A':reg+=0x00;break;
			case 'X':reg+=0x01;break;
			case 'L':reg+=0x02;break;
			case 'B':reg+=0x03;break;
			case 'S':reg+=0x04;break;
			case 'T':reg+=0x05;break;
			case 'F':reg+=0x06;break;
			default :cout<<"\nINVALID REGISTER USE IN LINE NO. :"<<index;
				 exit(0);
		}
	}
	else
	{
		cout<<"\n ERROR IN LINE NO.  :"<<index;
		exit(0);
	}
	return reg;
}

/*-----------------------------------------------------------------------
	TO ADD THE MODIFICATIONS REQUIRED
-----------------------------------------------------------------------*/

void modifier(unsigned int starting)
{
	ifstream inp;
	inp.open("program.bin",ios::binary|ios::in);
	FILE *modify;
	modify=fopen("modify.txt","w");
	
	line temp;
	while(inp)
	{
		inp.read((char*)&temp,sizeof(temp));
		if(temp.obcode>0xFFFFFF)
		{
			fprintf(modify,"\nM^%.6X^05",temp.address-starting+1);
		}
	}
	fclose(modify);
	inp.close();
}

/*-------------------------------------------------------------------------
	TO CREATE THE OBJECT CODE FOR THE INPUT FILE
-------------------------------------------------------------------------*/

void PASS_2(unsigned int LOCCTR)
{
	unsigned int ini_add,eff_add,pos;
	unsigned int base,starting,line_buf,reset;	
	int displacement,index=0;	
	FILE *result;
	result=fopen("object_code.txt","w");	
	ifstream inp;
	inp.open("program.bin",ios::binary|ios::in);
//	to check if PASS_1 is complete
	if(!inp)
	{
		cout<<"\nTHE INTERMEDAITE FILE IS MISSING\nRUN PASS_1 BEFORE EXECUTING PASS_2";
		exit(0);
	}

	line temp,temp1;
	inp.read((char *)&temp,sizeof(temp));
	index++;
//	Printing the header record
	fprintf(result,"H^%-6.6s^%.6X^%.6X",temp.label,toHex(temp.operand),LOCCTR-toHex(temp.operand));
	
	inp.read((char *)&temp,sizeof(temp));
	index++;
	
	starting=ini_add=temp.address;
	fprintf(result,"\nT^%.6X^",temp.address);
	pos=ftell(result);
	fprintf(result,"  ^");
	
	while(inp)	
	{
		inp.read((char *)&temp1,sizeof(temp1));
		index++;
		if(strcmp(temp.mnemonic.name,"BASE")==0)
		{
			base=search(temp.operand,0);
		}		
		if(strcmp(temp.mnemonic.name,"RESW")==0||strcmp(temp.mnemonic.name,"RESB")==0)
		{
			fseek(result,pos,SEEK_SET);
			fprintf(result,"%.2X",line_buf);
			fseek(result,0,SEEK_END);

			while(strcmp(temp1.mnemonic.name,"RESW")==0||strcmp(temp.mnemonic.name,"RESB")==0)
			{	
				temp=temp1;
				inp.read((char *)&temp1,sizeof(temp1));
				index++;
			}	
		
			if(strcmp(temp.mnemonic.name,"END")==0)
			{							
				break;
			}
						
			ini_add=temp1.address;
			fprintf(result,"\nT^%.6X^",ini_add);
			pos=ftell(result);
			fprintf(result,"  ^");		
		}		
		if(temp1.address-ini_add>0x1E)
		{
			fseek(result,pos,SEEK_SET);
			fprintf(result,"%.2X",line_buf);
			fseek(result,0,SEEK_END);			
			ini_add=temp1.address;
			fprintf(result,"\nT^%.6X^",ini_add);
			pos=ftell(result);
			fprintf(result,"  ^");
		}
			
		if(temp.mnemonic.format==0)
		{
			if(strcmp(temp.mnemonic.name,"BYTE")==0)
				fprintf(result,"%.2X^",temp.obcode);
			else if(strcmp(temp.mnemonic.name,"WORD")==0)
				fprintf(result,"%.6X^",temp.obcode);
		}
		else if(temp.mnemonic.format==1)
		{
			fprintf(result,"%.2X^",temp.obcode);
		}
		else if(temp.mnemonic.format==2)
		{
			eff_add=registers(temp.operand,index);			
			temp.obcode+=eff_add;
			fprintf(result,"%.4X^",temp.obcode);	
		}
		else if(temp.mnemonic.format==3)
		{			
			if( strcmp(temp.mnemonic.name,"RSUB")==0)
			{
				fprintf(result,"%.6X^",temp.obcode);
				goto A;
			}			
			
			eff_add=search(temp.operand,index);			
			if(temp.obcode>0xFFFFFF)
			{
				temp.obcode+=eff_add;
				fprintf(result,"%.8X^",temp.obcode);
				goto A;
			}			
			if(!(temp.obcode & 0x020000) ) 
			{
				temp.obcode+=eff_add;
				fprintf(result,"%.6X^",temp.obcode);
				goto A;
			}
			displacement=eff_add-temp1.address;
		
			if(displacement<0x00)
			{
				displacement+=0x1000;
			}
			if(displacement<=0xFFF)
			{
				temp.obcode+=displacement+0x002000;			
			}
			else if(eff_add-base<=0xFFF)
			{
				temp.obcode+=eff_add-base+0x004000;
			}
			else
			{
				cout<<"\n LOCATION COUNTER ERROR IN LINE NO. :"<<index;
				exit(0);
			}
			fprintf(result,"%.6X^",temp.obcode);
		}
	A:	line_buf=temp1.address-ini_add;
		temp=temp1;		
	}
			
	inp.close();
	modifier(starting);
	
	char ch;
	ifstream modify;
	modify.open("modify.txt",ios::in);

	while(modify.get(ch))
	{
		fprintf(result,"%c",ch);
	}

	modify.close();	
	fprintf(result,"\nE^%.6X",starting);
	fclose(result);
}
/*--------------------------------------------------------------------------------------------------
	MAIN()
--------------------------------------------------------------------------------------------------*/
int main()
{
	unsigned int LOCCTR=PASS_1();
	cout<<"\n SYMTAB : \n";	
	symtab();		
	PASS_2(LOCCTR);	
	return 0;
}
