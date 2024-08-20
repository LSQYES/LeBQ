#pragma once
#include <algorithm>
#include <fstream>
#include<math.h>
#include "bplustree.h"

#define DEBUGGER 0

using namespace std;

bool compareRecords(const Record* a, const Record* b) {
	return a->key < b->key;
}

void bulkLoadingInsert(Node*& tree, vector<Record*> data, int treeOrder) {
	size_t count = 0;
	vector<Node*> nodes;
	vector<KeyType> upperKeys;
	upperKeys.clear();
	vector<Node*> upperPointers;
	upperPointers.clear();

	sort(data.begin(), data.end(), compareRecords);

	while (count < data.size()) {
		Node* temporaryNode = new Node(treeOrder, true);
		temporaryNode->paths.clear();
		try {
			KeyType tempKey;
			tempKey = data[count];
			while (temporaryNode->numberOfKeys < (treeOrder - 1) / 2 && count < data.size()) {
				tempKey = data[count];
				if (temporaryNode->numberOfKeys > 0 && !(data[count]->key >
					temporaryNode->keys[temporaryNode->numberOfKeys - 1]->key)) {
					temporaryNode->paths[temporaryNode->paths.size() - 1].push_back(tempKey);
				}
				else {
					vector<KeyType> newVector;
					temporaryNode->keys.push_back(tempKey);
					newVector.push_back(tempKey);
					temporaryNode->paths.push_back(newVector);
					temporaryNode->numberOfKeys++;
				}
				count++;
			}
			if (nodes.size() > 0) {
				nodes[nodes.size() - 1]->nextList = temporaryNode;
				temporaryNode->lastList = nodes[nodes.size() - 1];
			}
			// 将当前节点添加到节点向量中
			nodes.push_back(temporaryNode);
			// 处理具有相同键值的记录
			while (count < data.size() && nodes.size() > 0 && !(data[count]->key > nodes[nodes.size() - 1]
				->keys[nodes[nodes.size() - 1]->numberOfKeys - 1]->key)) {
				tempKey = data[count];
				nodes[nodes.size() - 1]->paths[temporaryNode->paths.size() - 1].push_back(tempKey);
				count++;
			}
		}
		catch (...) {
			cout << "[Exception] function 'bulkloading': error during tree construct. this "
				"column doesn't exist in line :"
				<< count << "\n";
			return;
		}
	}

	if (nodes[0]->numberOfKeys < (treeOrder - 1) / 2) {
		tree = nodes[0];
		tree->parent = NULL;
		return;
	}

	if (nodes[nodes.size() - 1]->numberOfKeys < (treeOrder - 1) / 2) {
		for (int i = 0; i < nodes[nodes.size() - 1]->numberOfKeys; i++) {
			nodes[nodes.size() - 2]->keys.push_back(nodes[nodes.size() - 1]->keys[i]);
			nodes[nodes.size() - 2]->paths.push_back(
				nodes[nodes.size() - 1]->paths[i]);
			nodes[nodes.size() - 2]->numberOfKeys++;
		}
		nodes[nodes.size() - 2]->nextList = NULL;
		nodes.erase(nodes.begin() + nodes.size() - 1);
	}

	if (nodes.size() == 1) {
		tree = nodes[0];
		tree->parent = NULL;
		return;
	}

	for (size_t i = 1; i < nodes.size(); i++) {
		upperKeys.push_back(nodes[i]->keys[0]);
		upperPointers.push_back(nodes[i - 1]);
	}
	upperPointers.push_back(nodes[nodes.size() - 1]);
	upFirsts(upperKeys, upperPointers, treeOrder, tree);
}

void upFirsts(vector<KeyType>& keys, vector<Node*>& pointers, int treeOrder, Node*& tree) {
	vector<KeyType> upperKeys;
	vector<Node*> upperPointers;
	vector<Node*> level;

	size_t count = 0;
	while (count < keys.size()) {
		Node* temporaryNode = new Node(treeOrder);

		while (temporaryNode->numberOfKeys < (treeOrder - 1) / 2 &&
			count < keys.size()) {
			temporaryNode->keys.push_back(keys[count]);
			temporaryNode->pointers.push_back(pointers[count]);
			pointers[count]->parent = temporaryNode;
			temporaryNode->numberOfKeys++;
			count++;
		}

		pointers[count]->parent = temporaryNode;
		temporaryNode->pointers.push_back(pointers[count]);
		level.push_back(temporaryNode);

		if (keys.size() - count > ((size_t)treeOrder - 1) / 2) {
			upperKeys.push_back(keys[count]);
			upperPointers.push_back(level[level.size() - 1]);
			count++;
		}
		else {
			while (count < keys.size()) {
				level[level.size() - 1]->keys.push_back(keys[count]);
				level[level.size() - 1]->numberOfKeys++;
				level[level.size() - 1]->pointers.push_back(pointers[count + 1]);
				pointers[count + 1]->parent = level[level.size() - 1];
				count++;
			}
		}
	}

	upperPointers.push_back(level[level.size() - 1]);

	if (level.size() == 1) {
		tree = level[0];
		tree->parent = NULL;
		return;
	}

	upFirsts(upperKeys, upperPointers, treeOrder, tree);
}

vector<KeyType> LeBQP(Query* query, double delta, Node* tree)
{
	vector<KeyType> result;
	vector<NodePair> searchLenPair = getSearchNodePairByDifLen(query, delta, tree);
	int* bucketArrOfQueryKey = getCountBucketNumber(query->key);
	int difValue = floor((1 - delta) * query->elems.size() / delta);
	Node* searchNode;

	for (int i = 0; i < searchLenPair.size(); i++)
	{
		searchNode = searchLenPair[i].lenHead;
		while (searchNode != searchLenPair[i].lenRear)
		{
			for (int j = 0; j < searchNode->keys.size(); j++)
			{
				int* bucketArrOfLeafKey = getCountBucketNumber(searchNode->keys[j]->key);
				int difCount = 0;
				bool isCandidate = true;
				for (int n = 0; n < BUCKET_NUMBER; n++)
				{
					int currentDif = abs(bucketArrOfQueryKey[n] - bucketArrOfLeafKey[n]);
					difCount += currentDif;
					if (currentDif > difValue || difCount > difValue) {
						isCandidate = false;
						break;
					}
				}
				if (isCandidate)
				{
					for (int k = 0; k < searchNode->paths[j].size(); k++)
					{
						if (getJaccard(searchNode->paths[j][k], query) >= delta)
						{
							result.push_back(searchNode->paths[j][k]);
						}
					}
				}
				free(bucketArrOfLeafKey);
			}
			searchNode = searchNode->nextList;
		}
		for (int j = 0; j < searchNode->keys.size(); j++)
		{
			int* bucketArrOfLeafKey = getCountBucketNumber(searchNode->keys[j]->key);
			int difCount = 0;
			bool isCandidate = true;
			for (int n = 0; n < BUCKET_NUMBER; n++)
			{
				int currentDif = abs(bucketArrOfQueryKey[n] - bucketArrOfLeafKey[n]);
				difCount += currentDif;
				if (currentDif > difValue || difCount > difValue) {
					isCandidate = false;
					break;
				}
			}
			if (isCandidate)
			{
				for (int k = 0; k < searchNode->paths[j].size(); k++)
				{
					if (getJaccard(searchNode->paths[j][k], query) >= delta)
					{
						result.push_back(searchNode->paths[j][k]);
					}
				}
			}
			free(bucketArrOfLeafKey);
		}
	}
	return result;
}

vector<NodePair> getSearchNodePairByDifLen(Query* query, double delta, Node* tree) {
	vector<NodePair> result;
	int elemsLen = query->elems.size();
	int* queryBucketNumber = getCountBucketNumber(query->key);
	int difValue = floor((1 - delta) * elemsLen / delta);
	int minLen = elemsLen - difValue;
	int maxLen = elemsLen + difValue;
	if (maxLen > 255)
		maxLen = 255;

	for (int i = minLen; i <= maxLen; i++)
	{
		NodePair newpair;
		newpair.len = i;
		int minCoverage = ceil((elemsLen + i) * delta / (1 + delta));
		if (i < minCoverage)
		{
			continue;
		}

		newpair.lenHead = findHeadNodeByKey(getHeadKey(queryBucketNumber, elemsLen, minCoverage, i), tree);
		newpair.lenRear = findRearNodeByKey(getRearKey(queryBucketNumber, elemsLen, minCoverage, i), tree);
		result.push_back(newpair);
	}
	return result;
}

unsigned long long getHeadKey(int countBucketNumberTemp[], int elemsLen, int minCoverage, int queryLen) {
	int bucketBit = (WORD_SIZE - ELEM_LENGTH) / BUCKET_NUMBER;
	int blankNum = WORD_SIZE - ELEM_LENGTH - bucketBit * BUCKET_NUMBER;

	int countBucketNumber[BUCKET_NUMBER];
	for (int i = 0; i < BUCKET_NUMBER; i++)
	{
		countBucketNumber[i] = countBucketNumberTemp[i];
	}
	int difValue = elemsLen - minCoverage;
	int n = 0;
	while (n < BUCKET_NUMBER)
	{
		if (countBucketNumber[n] - difValue >= 0)
		{
			countBucketNumber[n] -= difValue;
			break;
		}
		else
		{
			difValue -= countBucketNumber[n];
			countBucketNumber[n] = 0;
			n++;
		}
	}

	unsigned long long word = 0;
	int curLen = 0;
	for (int i = 0; i < BUCKET_NUMBER; i++)
	{
		word <<= bucketBit;
		word |= countBucketNumber[i];
		curLen += bucketBit;
	}
	return  (word << blankNum) | getLenkeyByElemslen(queryLen);
}

unsigned long long getRearKey(int countBucketNumberTemp[], int elemsLen, int minCoverage, int queryLen) {
	int bucketBit = (WORD_SIZE - ELEM_LENGTH) / BUCKET_NUMBER;
	int blankNum = WORD_SIZE - ELEM_LENGTH - bucketBit * BUCKET_NUMBER;

	int countBucketNumber[BUCKET_NUMBER];
	for (int i = 0; i < BUCKET_NUMBER; i++)
	{
		countBucketNumber[i] = countBucketNumberTemp[i];
	}
	int difValue = elemsLen - minCoverage;
	int n = BUCKET_NUMBER - 1;
	while (n >= 0)
	{
		if (countBucketNumber[n] - difValue >= 0)
		{
			countBucketNumber[n] -= difValue;
			break;
		}
		else
		{
			difValue -= countBucketNumber[n];
			countBucketNumber[n] = 0;
			n--;
		}
	}

	unsigned long long word = 0;
	int curLen = 0;
	for (int i = 0; i < BUCKET_NUMBER; i++)
	{
		word <<= bucketBit;
		word |= countBucketNumber[i];
		curLen += bucketBit;
	}
	return  (word << blankNum) | getLenkeyByElemslen(queryLen);

}




Node* findHeadNodeByKey(unsigned long long searchKey, Node* tree) {
	Node* searchNode = tree;
	while (!searchNode->isLeaf) {
		for (int i = 0; i < searchNode->keys.size(); i++)
		{
			if (searchNode->keys[i]->key > searchKey) {
				searchNode = searchNode->pointers[i];
				break;
			}
			if (i == searchNode->keys.size() - 1)
			{
				searchNode = searchNode->pointers[i + 1];
				break;
			}
		}
	}
	return searchNode;
}

Node* findRearNodeByKey(unsigned long long searchKey, Node* tree) {
	Node* searchNode = tree;
	while (!searchNode->isLeaf) {
		for (int i = 0; i < searchNode->keys.size(); i++)
		{
			if (searchNode->keys[i]->key > searchKey) {
				searchNode = searchNode->pointers[i];
				break;
			}
			if (i == searchNode->keys.size() - 1)
			{
				searchNode = searchNode->pointers[i + 1];
				break;
			}
		}
	}
	return searchNode;
}
