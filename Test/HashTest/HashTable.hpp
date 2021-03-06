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

    int insert(unsigned int key, unsigned int tid, unsigned int offset);

    // Returns all the offsets for Journal
    DArray<DArray<unsigned>>* get(unsigned key);

    // Mask
    int getBucketIndex(int hash, int depth);

    unsigned getsize();

    // Returns offset for Journal
    int getLastJournalInsert(unsigned key);

    void deleteKey(unsigned key);

};


#endif
