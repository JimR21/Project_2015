#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP
#include <bitset>
#include "DArray.hpp"

#define HASHTABLE_SIZE 16

class Bucket;

class HashTable
{
    friend class Bucket;
private:
    // TODO: deikths se arxh journal

    unsigned size;

    DArray<Bucket*> bucketArray;  // Pointers to Buckets

    // Returns hashed number
    int hashFunction(unsigned int key);

    //Doubles table to a prime
    void doubleTableSize();

protected:
    unsigned int globalDepth;


public:

    HashTable();

    ~HashTable();

    int insert(unsigned int key, int data);

    // Returns Bucket address
    int get(unsigned int key);
    int getBucketIndex(int hash, int depth);

    unsigned getsize();

};


#endif
