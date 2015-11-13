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
    // #CHANGE:10 Change to DArray
    // #TODO:40 Delete

    /*for(std::vector<Bucket*>::iterator it = bucketArray.begin(); it != bucketArray.end(); ++it)
    {
        delete *it;
    }*/
}

int HashTable::hashFunction(unsigned int key)
{
    return (key * 2654435761 % 4294967296);    // Knuth: hash(i)=i*2654435761 mod 2^32
}

void HashTable::doubleTableSize()
{
    globalDepth++;
    unsigned new_size = size*2;
    for(unsigned i = size; i < new_size; i++)
    {
        bucketArray.push_back(bucketArray[i-size]);
    }
    size = new_size;
}

int HashTable::insert(unsigned int key, int data)
{
    unsigned hashed_key;
    hashed_key = hashFunction(key);
    Bucket* tempBucket = bucketArray[hashed_key % size];
    if(tempBucket->empty == true)
    {
        tempBucket->insert(key, data);
    }
    else
    {
        if(tempBucket->key == key)
        {
            // #TODO:10 Array of tids
        }
        else
        {
            // #TODO:70 Update with local and globalDepth
            unsigned bhashed_key = hashFunction(tempBucket->key);  // Bucket hashed key
            while(bhashed_key % size == hashed_key % size)
            {
                doubleTableSize();
            }
            if(bhashed_key % size != hashed_key % size)
            {
                bucketArray[hashed_key % size] = new Bucket(key, data, globalDepth);     // #DONE:60 New constructor
                bucketArray[bhashed_key % size]->localDepth++;
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

int HashTable::get(int key)
{
    unsigned hashed_key;
    hashed_key = hashFunction(key) % size;
    Bucket* tempBucket = bucketArray[hashed_key];
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
