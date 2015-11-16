#include "Bucket.hpp"
#include "HashTable.hpp"
#include <iostream>

using namespace std;

HashTable::HashTable()
{
    globalDepth = 7;    // HASHTABLE_SIZE 97 and 2^7 = 128
    size = HASHTABLE_SIZE;

    // Initiate bucketArray
    // #CHANGE:0 Change to DArray
    for(unsigned i = 0; i < HASHTABLE_SIZE; i++)
    {
        bucketArray.push_back(new Bucket());
    }

}

HashTable::~HashTable()
{
    // Delete bucketArray
    // #CHANGE:20 Change to DArray
    // #TODO:40 Delete

    /*unsigned size = bucketArray.size();
    for(unsigned i = 0; i < size-1; i++)
    {
        delete bucketArray[i];
    }*/
}

int HashTable::hashFunction(unsigned int key)
{
    return (key * 2654435761 % 4294967296);    // Knuth: hash(i)=i*2654435761 mod 2^32
}

int HashTable::getBucketIndex(int hash, int depth)
{
	return hash & ( (1 << depth) - 1);
}


void HashTable::doubleTableSize()
{
    globalDepth++;
    unsigned new_size = size*2;
    for(unsigned i = size; i < new_size; i++)
    {
        bucketArray.push_back(bucketArray.get(i-size));
    }
    size = new_size;
}

int HashTable::insert(unsigned int key, int data)
{
    unsigned hashed_key;
    hashed_key = hashFunction(key);
    int index = getBucketIndex(hashed_key, globalDepth); // koitaw ta globaldepth deksia bits gia na dw se poio index tha paw
    Bucket* tempBucket = bucketArray.get(index);
    if(tempBucket->empty == true)
    {
        tempBucket->insert(key, data);
    }
    else
    {
        if(tempBucket->key == key)
        {
            // #TODO:0 Array of tids
        }
        else
        {
            // #DONE:30 Update with local and globalDepth
            unsigned bhashed_key = hashFunction(tempBucket->key);  // Bucket hashed key
            while(getBucketIndex(bhashed_key, globalDepth) == getBucketIndex(hashed_key, globalDepth))
            {
                doubleTableSize();
            }
            int index2 = getBucketIndex(bhashed_key, globalDepth);
            index = getBucketIndex(hashed_key, globalDepth);
            if(index != index2)
            {
                bucketArray.set(index, new Bucket(key, data, globalDepth));     // #DONE:0 New constructor
                bucketArray.get(index2)->localDepth++;
            }
            else
            {
                cout << "Error: HashTable::insert" << endl;
                return -1;
            }
        }
    }
    return 1;
}

int HashTable::get(unsigned key)
{
    unsigned hashed_key;
    hashed_key = hashFunction(key) % size;
    Bucket* tempBucket = bucketArray.get(hashed_key);
    if((tempBucket->empty == false) && (tempBucket->key == key))
        return tempBucket->data;
    else
        cout << "Key not found" << endl;
    return -1;
}

unsigned HashTable::getsize()
{
    return size;
}
