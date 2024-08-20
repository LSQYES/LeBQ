#ifndef DATA_H
#define DATA_H
#include <stdio.h>
#include <set>
#include <vector>
#include <map>
#include "record.h"
#include "util.h"

using namespace std;

class Data
{
public:

	Data(char* filename);
	Data(char* datafilename, char* distinctfilename, char* rawfilename, int t = 0);
	~Data();

	Query* readNextQuery();
	Record* readNextRecord();
	void readData();
	void readQuery(char* queryfile);

	int* getElemsBuckets();


public:
	FILE* in;
	char* datafile;
	char* distinctfile;
	char* rawfile;
	int current;
	int type;
	//int transcount;
	set<int> distinctTerms;
	int maxElem;
	int minElem;
	int minLen;
	int maxLen;

	vector<Record*>  vec_trans;
	vector<Query*>  vec_querys;
	vector<Record*> vec_longset;
};

#endif DATA_H