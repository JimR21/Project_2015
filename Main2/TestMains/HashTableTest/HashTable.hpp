#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP
#include <bitset>
#include "DArray.hpp"

#define HASHTABLE_SIZE 128
#define STARTING_GLOBAL_DEAPTH 7

class Bucket;

class HashTable
{
    friend class Bucket;
private:
    int maxLocalCounter;    // Posa buckets exoun localDepth = globalDepth

    unsigned size;

    DArray<Bucket*> bucketArray;  // Pointers to Buckets

    // Returns hashed number
    int hashFunction(unsigned int key);

    void doubleTableSize();

    void halveTableSize();

protected:
    unsigned int globalDepth;


public:

    HashTable();

    ~HashTable();

    void insert(unsigned key, unsigned int tid, unsigned int offset);

	DArray<int>* getHashRecord(unsigned key, uint64_t start_tid, uint64_t end_tid);

	DArray<int>* getHashRecords(unsigned key);

	bool existCheck(unsigned key,uint64_t, uint64_t);

    // Mask
    int getBucketIndex(int hash, int depth);

    // Returns offset for Journal
	int getLastJournalInsert(unsigned key);

    unsigned getsize();

    int deleteKey(unsigned key);
};


#endif
