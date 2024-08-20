#include <iostream>
#include <string>
#include <chrono>
#include <filesystem>
#include <math.h>
#include "bplustree.h"
#include "util.h"
#include "data.h"
#include "node.h"

using namespace std;

double delta = 0.9;
int treeOrder = 16;
int maxBits = (WORD_SIZE - ELEM_LENGTH) / BUCKET_NUMBER;


int main() {
	char datafile[200];
	char queryfile[200];

	Data* data = new Data(datafile);
	data->readData();
	data->readQuery(queryfile);

	int* elemsBuckets = data->getElemsBuckets();

	for (int i = 0; i < data->vec_trans.size(); i++)
	{
		generateKey(data->vec_trans[i]->elems, data->vec_trans[i]->key, maxBits, elemsBuckets);
	}

	Node* tree = NULL;
	bulkLoadingInsert(tree, data->vec_trans, treeOrder);
	vector<KeyType> result;
	auto start = chrono::high_resolution_clock::now();
	for (int i = 0; i < data->vec_querys.size(); i++)
	{
		int queryLen = data->vec_querys[i]->elems.size();
		for (int n = 0; n < data->vec_longset.size(); n++)
			if (getJaccard(data->vec_longset[n], data->vec_querys[i]) >= delta)
				result.push_back(data->vec_longset[n]);
		if (ceil(queryLen * delta) > 255)
			continue;
		generateKey(data->vec_querys[i]->elems, data->vec_querys[i]->key, maxBits, elemsBuckets);
		vector<KeyType> resultTemp = LeBQP(data->vec_querys[i], delta, tree);
		result.insert(result.end(), resultTemp.begin(), resultTemp.end());
	}
	auto end = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
	cout << endl << "average search time：" << (float)duration.count() / 1000 / data->vec_querys.size() << "ms" << endl;
	cout << result.size();
}