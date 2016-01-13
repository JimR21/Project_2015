#ifndef VAL_HASHTABLE_HPP
#define VAL_HASHTABLE_HPP
//#include <string>
#include "DArray.hpp"

#define HASHTABLE_SIZE 128
#define STARTING_GLOBAL_DEAPTH 7
#define BUCKET_OVERFLOW 4

class BucketVal;

class Val_HashTable
{
	friend class BucketVal;
protected:
	DArray<int> maxLocalCounter;                 // Posa buckets exoun localDepth = globalDepth

    unsigned size;

    unsigned int globalDepth;

	DArray<BucketVal*> Buckets;                 // Pointers to Buckets

	std::hash<std::string> hashFunction;

	uint32_t getBucketIndex(uint64_t hash, int depth);

	void doubleTableSize();		//override giati exw new array

	void halveTableSize();

	bool splitcheck(uint32_t index, uint32_t depth);

	void split(uint32_t index, uint32_t depth, BucketVal* newbucket, std::string key, unsigned range);

	bool popKey(BucketVal* bucket, std::string key);

public:
	// DEBUG
    unsigned inserts;

	Val_HashTable();

	~Val_HashTable();

	unsigned getsize();

	//int hashFunction(const std::string& key);
    void insert(std::string key, unsigned range);

	int deleteKey(std::string key);

    char* getbdata(std::string key);

	bool datacheck(BucketVal* bucket);

	void UpdateValData(std::string key, DArray<bool> *array);
};


#endif
