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
	string label;
	instruction mnemonic;
	string operand;
	unsigned int obcode;
	unsigned int address;	
	
};

/*------------------------------------------------------------------------------------------
	To store the symbols/labels used in the program.
------------------------------------------------------------------------------------------*/
struct symbol
{
	string label;
	unsigned int address;
};

/*------------------------------------------------------------------------------------------
	TO PERFORM LINEAR SEARCH IN THE OPTAB
------------------------------------------------------------------------------------------*/
int linearsearch(instruction ob[], const char key[])
{
	int i=0;
	while (i<62)
	{
		if(strcmp(key,ob[++i].name)==0)
		break;
	}
	return i;
}


int toHex(string str)
{	
	stringstream s(str);
	int value;
	s>>hex>>value;
	return value;
}

int toDec(string str)
{	
	stringstream s(str);
	int value;
	s>>value;
	return value;
}

string toString(int val)
{
	string result;
	stringstream convert;
	convert<<val;
	result=convert.str();
	return result;
}

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

		if(temp.label==cur_label)
		{
			found=1;
			break;
		}						
	}
	
	sym.close();
	if(found==0)
	{	
		constant=toDec(cur_label);		
		if(constant!=0)
			return constant;
		else if(constant==0 && cur_label[0]=='0')
			return constant;
		else
		{
			cout<<"\n UNDEFINED LABEL USED IN LINE NO. :"<<index;
			exit(0);
		}		
	}
	return temp.address;
}

int main()
{
	cout<<search("LENGTH",5);
	return 0;
}
