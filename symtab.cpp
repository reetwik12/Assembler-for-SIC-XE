#include<iostream>
#include<stdio.h>
#include<fstream>
#include<cstdlib>
#include<string>
#include<cstring>
#include<sstream>
#include<cmath>
using namespace std;


void symtab()
{
	ifstream search;
	search.open("sym_tab.bin",ios::binary|ios::in);
	while(search)
	{
		symbol s1;
		search.read((char *)&s1,sizeof(s1));			
		cout<<s1.label<<"\t"<<s1.address<<endl;				
	}
	search.close();
			
}
