#pragma once
#include <vector>

using namespace std;

struct Query
{
public:
	vector<int> elems;
	unsigned long long key;
};

class Record
{
public:
	int id;
	vector<int> elems;
	unsigned long long key;
};

