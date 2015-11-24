#include "Bucket.hpp"
#include "HashTable.hpp"
#include <iostream>

using namespace std;

//=======================================================================================================
HashTable::HashTable()
{
    globalDepth = 7;    // HASHTABLE_SIZE 97 and 2^7 = 128
    size = HASHTABLE_SIZE;

	// cout << "Hash Table created!" << endl;

    for(unsigned i = 0; i < HASHTABLE_SIZE; i++)
        bucketArray.push_back(new Bucket());
}
//=======================================================================================================
HashTable::~HashTable()
{
    // Delete bucketArray
    unsigned size = bucketArray.size();
    for(unsigned i = 0; i < size; i++)
    {
        delete bucketArray.get(i);
    }
}
//=======================================================================================================
int HashTable::hashFunction(unsigned int key)
{
    return (key * 2654435761 % 4294967296);    // Knuth: hash(i)=i*2654435761 mod 2^32
}
//=======================================================================================================
int HashTable::getBucketIndex(int hash, int depth)
{
	return hash & ( (1 << depth) - 1);
}
//=======================================================================================================
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
//=======================================================================================================
int HashTable::insert(unsigned int key, unsigned tid, unsigned offset)
{
    unsigned hashed_key;
    hashed_key = hashFunction(key);
    int index = getBucketIndex(hashed_key, globalDepth); // koitaw ta globaldepth deksia bits gia na dw se poio index tha paw
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
                doubleTableSize();

            int index2 = getBucketIndex(bhashed_key, globalDepth);
            index = getBucketIndex(hashed_key, globalDepth);

            bucketArray.set(index, new Bucket(key, data, globalDepth));     // #DONE:0 New constructor
            bucketArray.get(index2)->localDepth++;
        }
    }
    return 1;
}
//=======================================================================================================
DArray<DArray<unsigned>>*  HashTable::getHashRecord(unsigned key, uint64_t start_tid, uint64_t end_tid){
	unsigned hashed_key;
    hashed_key = hashFunction(key);
	int index = getBucketIndex(hashed_key, globalDepth); // koitaw ta globaldepth deksia bits gia na dw se poio index tha paw
    Bucket* tempBucket = bucketArray.get(index);

	DArray<DArray<unsigned>>* array = new DArray<DArray<unsigned>>();

	if((tempBucket->empty == false) && (tempBucket->key == key)){
		for (int i = 0; i < tempBucket->data.size(); i++){

			uint64_t tid = tempBucket->data.get(i)->getTid();	// pare to tid tou current bucketData

			if (tid >= start_tid && tid <= end_tid)	// an einai mesa sto range valto ston pinaka
				array->push_back(tempBucket->data.get(i)->rangeArray);
			else {
				cout << "Out of range" << endl;
				break;
			}
		}
		return array;
	}
	else
        cout << "Key not found" << endl;
	return NULL;
}
//=======================================================================================================
DArray<DArray<unsigned>>*  HashTable::getHashRecords(unsigned key)
{
    unsigned hashed_key;
    hashed_key = hashFunction(key);
	int index = getBucketIndex(hashed_key, globalDepth); // koitaw ta globaldepth deksia bits gia na dw se poio index tha paw
    Bucket* tempBucket = bucketArray.get(index);

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
//=======================================================================================================
unsigned HashTable::getsize()
{	return size; }
//=======================================================================================================
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
//=======================================================================================================
