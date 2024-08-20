#pragma once
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <time.h>
#include <vector>
#include "record.h"


using namespace std;

#define BUCKET_NUMBER 8 //分割的桶的数量
#define ELEM_LENGTH 8  //前ELEM_LENGTH为key中长度所占位数
#define WORD_SIZE (sizeof(unsigned long long) * 8)

#define GetBit(v, n) ((v) & ((UINT64)1 << (n))) //获取v 的第 n 位

bool compareRecords(const Record* a, const Record* b);
double getJaccard(Record* rec, Query* que);
unsigned long long generateMask(int num_bits);
unsigned long long getLenkeyByElemslen(int elemsLen);
int* getCountBucketNumber(unsigned long long key);
void generateKey(vector<int> elems, unsigned long long& res_vec, int maxBits, int* elemsbucket);
