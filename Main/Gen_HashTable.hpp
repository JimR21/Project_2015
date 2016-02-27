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

    volatile unsigned int globalDepth;

    DArray<Bucket*> bucketArray;                 // Pointers to Buckets

    int hashFunction(unsigned int key);          // Returns hashed number

    uint32_t getBucketIndex(uint64_t hash, int depth);

    void doubleTableSize();

    void halveTableSize();

public:

    Gen_HashTable();

    ~Gen_HashTable();

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
