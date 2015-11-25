#include "Bucket.hpp"
#include "HashTable.hpp"
#include <math.h>
#include <iostream>

using namespace std;

HashTable::HashTable()
{
    globalDepth = 7;    // 2^7 = 128
    size = HASHTABLE_SIZE;
    maxLocalCounter = 128;

	// cout << "Hash Table created!" << endl;
    for(unsigned i = 0; i < HASHTABLE_SIZE; i++)
        bucketArray.push_back(new Bucket());
}

HashTable::~HashTable()
{
    // Delete bucketArray
    for(unsigned i = 0; i < size; i++)
    {
        if(bucketArray.get(i) == NULL)     // Elegxos gia na mh svistei 2 fores to idio
            delete bucketArray.get(i);
    }
}

int HashTable::hashFunction(unsigned int key)
{
    //return (key * 2654435761 % 4294967296);    // Knuth: hash(i)=i*2654435761 mod 2^32
    return key;
}

int HashTable::getBucketIndex(int hash, int depth)
{
	//return hash & ( (1 << depth) - 1);
    int p = pow(2, depth);
    return (hash % p);
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

int HashTable::insert(unsigned int key, unsigned tid, unsigned offset)
{
    unsigned hashed_key;
    hashed_key = hashFunction(key);
    int index = getBucketIndex(hashed_key, globalDepth); // koitaw ta globalDepth deksia bits gia na dw se poio index tha paw
    Bucket* tempBucket = bucketArray.get(index);

	// create BucketData to store
	BucketData* data = new BucketData(tid, offset);


    if(tempBucket->empty == true)
    {
        tempBucket->insert(key, data);
    }
    else
    {
        if(tempBucket->key == key)
        {
			bool done = false;
			for(int i=0;i<tempBucket->data.size();i++) {
				if(tempBucket->data.get(i)->tid == tid) {
					tempBucket->data.get(i)->rangeArray.push_back(offset);
					done = true;
					break;
				}
			}
			if(!done) {
		        tempBucket->insert(key, data);
			}

        }
        else
        {
            unsigned bhashed_key = hashFunction(tempBucket->key);  // Bucket hashed key
            while(getBucketIndex(bhashed_key, globalDepth) == getBucketIndex(hashed_key, globalDepth))
            {
                doubleTableSize();
                maxLocalCounter = 0;
            }
            int index2 = getBucketIndex(bhashed_key, globalDepth);
            index = getBucketIndex(hashed_key, globalDepth);

            bucketArray.set(index, new Bucket(key, data, globalDepth));     // #DONE:0 New constructor
            maxLocalCounter++;

            bucketArray.get(index2)->localDepth++;
            if(bucketArray.get(index2)->localDepth == globalDepth)
                maxLocalCounter++;
            // TESTING
            if(bucketArray.get(index2)->localDepth > globalDepth)
            {
                cout << "Problem2!!!!! : offset = " << offset << endl;
            }
        }
    }
    return 1;
}

DArray<DArray<unsigned>>*  HashTable::get(unsigned key)
{
    unsigned hashed_key;
    hashed_key = hashFunction(key);
	int index = getBucketIndex(hashed_key, globalDepth); // koitaw ta globalDepth deksia bits gia na dw se poio index tha paw
    Bucket* tempBucket = bucketArray.get(index);

	hashed_key = hashFunction(key);

	DArray<DArray<unsigned>>* array = new DArray<DArray<unsigned>>();

    if((tempBucket->empty == false) && (tempBucket->key == key)){
		for (int i = 0; i < tempBucket->data.size(); i++){
			array->push_back(tempBucket->data.get(i)->rangeArray);
		}
		return array;
	}
    else
        cout << "Key not found" << endl;
	return NULL;
}

unsigned HashTable::getsize()
{
    return size;
}

int HashTable::getLastJournalInsert(unsigned key)  // NEW
{
    unsigned hashed_key;
    hashed_key = hashFunction(key);
    unsigned idx = getBucketIndex(hashed_key, globalDepth);
    Bucket* tempBucket = bucketArray.get(idx);

    //  TESTING IF
    if((tempBucket->empty == false) && (tempBucket->key == key))
    {
        BucketData* bdata = tempBucket->getdataLast();
        return bdata->getrangeArrayLast();
    }
    else
        cout << "Key not found" << endl;
    return -1;
}

void HashTable::deleteKey(unsigned key)
{
    unsigned hashed_key;
    hashed_key = hashFunction(key);
    unsigned idx = getBucketIndex(hashed_key, globalDepth);
    Bucket* tempBucket = bucketArray.get(idx);

    if((tempBucket->empty == false) && (tempBucket->key == key))
    {
        if(tempBucket->localDepth < globalDepth || globalDepth == 7)    // An einai local < global tote kane empty to bucket
        {
            tempBucket->empty = true;
            tempBucket->key = 0;
        }
        else                                                   // An local = global tote vres
        {
            unsigned idx2 = getBucketIndex(hashed_key, globalDepth-1);
            if(idx2 == idx || idx2 < 128)
            {
                tempBucket->empty = true;
                tempBucket->key = 0;
            }
            else
            {
                if(bucketArray.get(idx2)->localDepth == globalDepth)
                    maxLocalCounter--;
                bucketArray.get(idx2)->localDepth--;
                bucketArray.set(idx, bucketArray.get(idx2));

                delete tempBucket;
                maxLocalCounter--;

                // TESTING
                if(maxLocalCounter == -1)
                {
                    cout << "Here" << endl;
                }

                if(maxLocalCounter == 0)
                {
                    globalDepth--;
                    size = size/2;
                    //bucketArray.get(idx2)->localDepth--;
                }
            }
        }
    }
    else
        cout << "Delete: Key not found" << endl;
}
