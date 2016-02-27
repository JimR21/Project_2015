#include "Bucket.hpp"
#include "Gen_HashTable.hpp"
#include <math.h>
#include <iostream>

using namespace std;

//=======================================================================================================
Gen_HashTable::Gen_HashTable()
{
    globalDepth = STARTING_GLOBAL_DEAPTH;    // 2^7 = 128
    size = HASHTABLE_SIZE;
    maxLocalCounter.push_back(HASHTABLE_SIZE);

    for(unsigned i = 0; i < HASHTABLE_SIZE; i++)
        bucketArray.push_back(new Bucket());
}
//=======================================================================================================
Gen_HashTable::~Gen_HashTable()
{
    // Delete bucketArray
    for(unsigned i = 0; i < size; i++)
    {
        if(bucketArray.get(i) != NULL)
        {
            unsigned p = pow(2, bucketArray.get(i)->localDepth);
            delete bucketArray.get(i);
            for(unsigned j = i; j < size; j+=p)
                bucketArray.set(j, NULL);
        }
    }
}

//=======================================================================================================
int Gen_HashTable::hashFunction(unsigned int key)
{
    return (key * 2654435761 % 4294967296);    // Knuth: hash(i)=i*2654435761 mod 2^32
}
//=======================================================================================================
uint32_t Gen_HashTable::getBucketIndex(uint64_t hash, int depth)
{
	return hash & ( (1 << depth) - 1);
}
//=======================================================================================================
void Gen_HashTable::halveTableSize()
{
    globalDepth--;
    size /= 2;
    for(unsigned i = 0; i < size; i++)
        bucketArray.popLast();
    maxLocalCounter.popLast();
}
//=======================================================================================================
unsigned Gen_HashTable::getsize()
{	return size; }
//=======================================================================================================
int Gen_HashTable::deleteKey(unsigned key)
{
    unsigned hashed_key;
    hashed_key = hashFunction(key);
    unsigned idx = getBucketIndex(hashed_key, globalDepth);
    Bucket* tempBucket = bucketArray.get(idx);

    if((tempBucket->empty == false) && (tempBucket->key == key))
    {
        if(tempBucket->localDepth < globalDepth)            // An einai local < global tote kane empty to bucket
        {
            tempBucket->empty = true;
            tempBucket->key = 0;
        }
        else                                                                    // An local = global
        {
            unsigned idx2;   // index to merge into

            // Find index of the bucket to merge into
            unsigned i = pow(2, globalDepth-1);
            if((idx & i) == 0)
                idx2 = idx + i;
            else
                idx2 = idx - i;

            Bucket* mergeBucket = bucketArray.get(idx2);
            mergeBucket->localDepth--;

            // Change the pointer to mergeBucket
            bucketArray.set(idx, mergeBucket);
            delete tempBucket;

            maxLocalCounter.set(maxLocalCounter.size()-1, maxLocalCounter.get(maxLocalCounter.size()-1)-2);
            if(maxLocalCounter.get(maxLocalCounter.size()-1) == 0)
                halveTableSize();
        }
        return 1;
    }
    return 0;
}
