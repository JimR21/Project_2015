#ifndef VAL_HASHTABLE_HPP
#define VAL_HASHTABLE_HPP
#include "Gen_HashTable.hpp"
#include <string>
#include <stdlib.h>

#define BUCKET_OVERFLOW 4

class BucketVal;

class Val_HashTable : public Gen_HashTable
{
	friend class BucketVal;
protected:

	std::hash<std::string> hashFunction;
	DArray<BucketVal*> Buckets;                 // Pointers to Buckets
public:
	Val_HashTable();
	//int hashFunction(const std::string& key);
    void insert(std::string key);
	void doubleTableSize();		//override giati exw new array
    int getbdata(std::string key);
	bool splitcheck(uint32_t index, uint32_t depth);
	void split(uint32_t index, uint32_t depth, BucketVal* newbucket);

};


#endif
