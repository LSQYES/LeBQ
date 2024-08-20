#include "util.h"

unsigned long long generateMask(int num_bits) {
	// 生成指定位数的全1掩码
	unsigned long long mask = (1ULL << num_bits) - 1;
	return mask;
}

unsigned long long getLenkeyByElemslen(int elemsLen) {
	unsigned long long lenkey = 0;
	for (int i = 0; i < ELEM_LENGTH; i++)
	{
		int wordPos = i + (WORD_SIZE - ELEM_LENGTH);
		unsigned int lenMask = 1 << i;
		if (lenMask & elemsLen)
		{
			lenkey |= 1ULL << wordPos;
		}
	}
	return lenkey;
}


void generateKey(vector<int> elems, unsigned long long& res_vec, int maxBits, int* elemsbucket)
{
	int elemsLen = elems.size();
	int countArr[BUCKET_NUMBER] = { 0 };
	int curLen = 0;
	unsigned long long word = 0;
	unsigned long long lenkey = 0;
	int blankNum = WORD_SIZE - ELEM_LENGTH - maxBits * BUCKET_NUMBER;

	for (int i = 0; i < ELEM_LENGTH; i++)
	{
		int wordPos = i + (WORD_SIZE - ELEM_LENGTH);
		unsigned int lenMask = 1 << i;

		if (lenMask & elemsLen)
		{
			lenkey |= 1ULL << wordPos;
		}

	}

	for (int i = 0; i < elems.size(); i++)
	{
		int no = elemsbucket[elems[i]];
		if (no < 0)
		{
			throw "unexpected elements encountered!";
		}
		else
		{
			countArr[no]++;
		}
	}

	for (int i = 0; i < BUCKET_NUMBER; i++)
	{
		if (curLen + maxBits <= WORD_SIZE)
		{
			word <<= maxBits;
			word |= countArr[i];
			curLen += maxBits;
		}
		else
		{
			int reminder = WORD_SIZE - curLen;
			word <<= reminder;
			word |= (countArr[i] >> (maxBits - reminder));
			res_vec = word;
			word = countArr[i] << (sizeof(int) * 8 - maxBits + reminder) >> (sizeof(int) * 8 - maxBits + reminder);
			curLen = maxBits - reminder;
		}
	}
	if (curLen != 0)
		res_vec = lenkey | (word << blankNum);
}

int* getCountBucketNumber(unsigned long long key) {
	int bucketBit = (WORD_SIZE - ELEM_LENGTH) / BUCKET_NUMBER;
	unsigned long long mask = generateMask(bucketBit) << (WORD_SIZE - ELEM_LENGTH - bucketBit);
	int* countBucketNumber = (int*)malloc(sizeof(int) * BUCKET_NUMBER);

	if (countBucketNumber == NULL) {
		return NULL;
	}
	for (int i = 0; i < BUCKET_NUMBER; i++) {
		int num = (key & mask) >> (WORD_SIZE - ELEM_LENGTH - bucketBit * (i + 1));
		countBucketNumber[i] = num;
		mask >>= bucketBit;
	}
	return countBucketNumber;
}

double getJaccard(Record* rec, Query* que) {

	vector<int> intersection;
	for (int elem : rec->elems)
	{
		if (find(que->elems.begin(), que->elems.end(), elem) != que->elems.end()) {
			intersection.push_back(elem);
		}
	}
	double union_size = rec->elems.size() + que->elems.size() - intersection.size();
	return intersection.size() / union_size;
}