#include "Bucket.hpp"
#include "HashTable.hpp"
#include <iostream>

using namespace std;

HashTable::HashTable()
{
    globalDepth = 1;    // HASHTABLE_SIZE 2 and 2^1 = 2
    size = HASHTABLE_SIZE;

    // Initiate bucketArray
    // #CHANGE:10 Change to DArray
    for(unsigned i = 0; i < HASHTABLE_SIZE; i++)
    {
        bucketArray.push_back(new Bucket());
    }

}

HashTable::~HashTable()
{
    // Delete bucketArray
    // #CHANGE:30 Change to DArray
    // #TODO:50 Delete

    /*for(std::vector<Bucket*>::iterator it = bucketArray.begin(); it != bucketArray.end(); ++it)
    {
        delete *it;
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
        bucketArray.push_back(bucketArray[i-size]);
    }
    size = new_size;
}

int HashTable::insert(unsigned int key, int data)
{
    unsigned hashed_key;
    hashed_key = hashFunction(key);
    int index = getBucketIndex(hashed_key, globalDepth); // koitaw ta globaldepth deksia bits gia na dw se poio index tha paw
    Bucket* tempBucket = bucketArray[index];
    if(tempBucket->empty == true)
    {
        cout << "==============================================" << endl;
        cout << "Bucket is empty. Inserting data with key -> " << key << endl;
        cout << "==============================================" << endl;

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
            cout << "==============================================" << endl;
            cout << "Trying to insert different key in full bucket " << endl;
            cout << "==============================================" << endl;

            // #DONE:40 Update with local and globalDepth
            unsigned bhashed_key = hashFunction(tempBucket->key);  // Bucket hashed key
            while(getBucketIndex(bhashed_key, globalDepth) == getBucketIndex(hashed_key, globalDepth))
            {
                doubleTableSize();
                cout << "DOUBLEING" << endl;
            }
            int index2 = getBucketIndex(bhashed_key, globalDepth);

            cout << "To neo index tou yparxodos bucket: " << index2 << endl;
            cout << "EKEI POU THA PAW = " << index2 << endl;
            cout << "EKEI POU HMOYN = " << index << endl;

            index = getBucketIndex(hashed_key, globalDepth);

            cout << "To neo index tou neou bucket: " << index << endl;
            if(index != index2)
            {
                bucketArray[index] = new Bucket(key, data, globalDepth);     // #DONE:10 New constructor
                bucketArray[index2]->localDepth++;
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

int HashTable::get(unsigned int key)
{
    unsigned hashed_key;
    hashed_key = hashFunction(key);
	int idx = getBucketIndex(hashed_key, globalDepth);
	cout << "Gia to key: " << key << " koitaw sto index: " << idx << endl;
    Bucket* tempBucket = bucketArray[idx];

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
