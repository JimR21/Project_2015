#ifndef GEN_HASHTABLE_HPP
#define GEN_HASHTABLE_HPP
#include "DArray.hpp"

#define HASHTABLE_SIZE 128
#define STARTING_GLOBAL_DEAPTH 7

class Bucket;

class Gen_HashTable
{
    friend class Bucket;
protected:
    DArray<int> maxLocalCounter;                 // Posa buckets exoun localDepth = globalDepth

    unsigned size;

    unsigned int globalDepth;

    DArray<Bucket*> bucketArray;                 // Pointers to Buckets

    int hashFunction(unsigned int key);          // Returns hashed number

    uint32_t getBucketIndex(uint64_t hash, int depth);

    void doubleTableSize();

    void halveTableSize();

public:

    // DEBUG
    unsigned inserts;

    Gen_HashTable();

    ~Gen_HashTable();

    // void insert(unsigned key, unsigned int tid, unsigned int offset);
    //
	// DArray<uint64_t>* getHashRecord(unsigned key, uint64_t start_tid, uint64_t end_tid);
    //
	// DArray<uint64_t>* getHashRecords(unsigned key);
    //
	// bool existCheck(unsigned key,uint64_t, uint64_t);
    //
    // // Mask
    //
    // // Returns offset for Journal
	// int getLastJournalInsert(unsigned key);

    unsigned getsize();

    int deleteKey(unsigned key);
};

inline void Gen_HashTable::doubleTableSize()
{
    globalDepth++;
    bucketArray.specialDouble();
    size *= 2;
    maxLocalCounter.push_back(0);        // Otan diplasiazetai o index sigoura de tha yparxei kanena bucket me localDepth = globalDepth

}
#endif
