#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>
#include "node.h"
#include "data.h"

#define _IDCOLUMN 0

using namespace std;

class TreeValue {
public:
	int maxBits = 0;
	Record* minRecord = nullptr;
	Record* maxRecord = nullptr;
};

struct NodePair
{
	int len;
	Node* lenHead;
	Node* lenRear;
};


void bulkLoadingInsert(Node*& tree, vector<Record*> data, int treeOrder);
void upFirsts(vector<KeyType>& keys, vector<Node*>& pointers, int treeOrder, Node*& tree);

vector<NodePair> getSearchNodePairByDifLen(Query* query, double delta, Node* tree);
vector<KeyType> LeBQP(Query* query, double delta, Node* tree);

Node* findHeadNodeByKey(unsigned long long searchKey, Node* tree);
Node* findRearNodeByKey(unsigned long long searchKey, Node* tree);

unsigned long long getHeadKey(int countBucketNumberTemp[], int elemsLen, int minCoverage, int queryLen);
unsigned long long getRearKey(int countBucketNumberTemp[], int elemsLen, int minCoverage, int queryLen);