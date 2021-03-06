#include "Bucket.hpp"
#include "HashTable.hpp"
#include <math.h>
#include <iostream>

using namespace std;

HashTable::HashTable()
{
    globalDepth = STARTING_GLOBAL_DEAPTH;    // 2^7 = 128
    size = HASHTABLE_SIZE;
    maxLocalCounter = HASHTABLE_SIZE;

    for(unsigned i = 0; i < HASHTABLE_SIZE; i++)
    {
        bucketArray.push_back(new Bucket());
    }
}

//================================================================================

HashTable::~HashTable()
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

//================================================================================

int HashTable::hashFunction(unsigned int key)
{
    return (key * 2654435761 % 4294967296);    // Knuth: hash(i)=i*2654435761 mod 2^32
}

//================================================================================

int HashTable::getBucketIndex(int hash, int depth)
{
	return hash & ( (1 << depth) - 1);
}

//================================================================================

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

//================================================================================

void HashTable::halveTableSize()    // TODO: Maybe a special function at DArray.hpp?
{
    globalDepth--;
    size /= 2;
    for(unsigned i = 0; i < size; i++)
        bucketArray.popLast();
}

//================================================================================

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
        // An to key yparxei...
        if(tempBucket->key == key)
        {
			bool done = false;
			for(int i=0;i<tempBucket->data.size();i++) {
				if(tempBucket->data.get(i)->tid == tid) {
                    // ... kai tid uparxei tote vale sto idio tid to offset...
					tempBucket->data.get(i)->rangeArray.push_back(offset);
					done = true;
					break;
				}
			}  // ...alliws vale to kainourio tid
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
                maxLocalCounter = 0;        // Otan diplasiazetai o index sigoura de tha yparxei kanena bucket me localDepth = globalDepth
            }
            int index2 = getBucketIndex(bhashed_key, globalDepth);
            index = getBucketIndex(hashed_key, globalDepth);

            if(tempBucket->localDepth == globalDepth -1)                        // Simple split otan uparxoun 2 pointers sto bucket
            {
                bucketArray.set(index, new Bucket(key, data, globalDepth));
                bucketArray.get(index2)->localDepth++;
                maxLocalCounter+=2;
            }
            else           // Split otan uparxoyn perissoteroi pointers sto bucket
            {
                unsigned local = bucketArray.get(index2)->localDepth;
                while(getBucketIndex(bhashed_key, local) == getBucketIndex(hashed_key, local))  // Oso to localDepth den einai arketo gia na diaxwristoun ta 2 kleidia, auksanetai kai diaxwrizontai ta buckets
                {
                    if(getBucketIndex(bhashed_key, local+1) != getBucketIndex(hashed_key, local+1)) // An sto localDepth+1 diaxwrizetai vges apo loop
                        break;
                    local++;
                    bucketArray.get(index2)->localDepth++;
                    Bucket* newBucket = new Bucket(local);                                        // Neo empty bucket

                    unsigned oldindex = getBucketIndex(hashed_key, local);
                    unsigned newindex;                                              // Ypologismos tou bucket index poy tha diaforopoieitai sto bit pou orizei to neo local
                    unsigned i = pow(2, local-1);
                    if((oldindex & i) == 0)
                        newindex = oldindex + i;
                    else
                        newindex = oldindex - i;
                    unsigned dist = pow(2, local);

                    for(unsigned i = newindex; i < size; i+=dist)   // Metafora deiktwn tou index sto newBucket
                    {
                        bucketArray.set(i, newBucket);
                    }
                }

                local++;
                Bucket* tempBucketnew = new Bucket(key, data, local);
                bucketArray.set(index, tempBucketnew);
                bucketArray.get(index2)->localDepth++;
                unsigned toindex = getBucketIndex(hashed_key, local);        // Ypologismos tou bucket index me to neo local depth
                unsigned dist = pow(2, local);                               // H apostash poy tha exei to bucket index me ton epomeno deikth poy tha deiksei sto new bucket
                for(unsigned i = toindex; i < size; i+=dist)                 // Metafora deiktwn tou indexTable sto tempBucketnew
                {
                    bucketArray.set(i, tempBucketnew);
                }
                if(local == globalDepth)   // An to local iso me global tote ta 2 bucket exoun localDepth = globalDepth kai to maxLocalCounter auksanetai kata 2
                    maxLocalCounter+=2;
            }
        }
    }
    return 1;
}

//================================================================================

DArray<DArray<unsigned>>*  HashTable::get(unsigned key)
{
    unsigned hashed_key;
    hashed_key = hashFunction(key);
	int index = getBucketIndex(hashed_key, globalDepth); // koitaw ta globalDepth deksia bits gia na dw se poio index tha paw
    Bucket* tempBucket = bucketArray.get(index);

	hashed_key = hashFunction(key);

	DArray<DArray<unsigned>>* array = new DArray<DArray<unsigned>>();

    // TESTING IF
    if((tempBucket->empty == false) && (tempBucket->key == key)){
		for (int i = 0; i < tempBucket->data.size(); i++){
			array->push_back(tempBucket->data.get(i)->rangeArray);
		}
		return array;
	}
    else
        cout << "get: Key not found" << endl;
	return NULL;
}

//================================================================================

unsigned HashTable::getsize()
{
    return size;
}

//================================================================================

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
        cout << "getLastJournalInsert: Key " << key << " not found" << endl;
    return -1;
}

//================================================================================

void HashTable::deleteKey(unsigned key)
{
    if(key == 492)
    {
        cout << "Stop" << endl;
    }
    unsigned hashed_key;
    hashed_key = hashFunction(key);
    unsigned idx = getBucketIndex(hashed_key, globalDepth);
    Bucket* tempBucket = bucketArray.get(idx);

    // TESTING IF
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

            maxLocalCounter-=2;
            if(maxLocalCounter == 0)
                halveTableSize();
        }
    }
    else
        cout << "Delete: Key " << key << " not found" << endl;
}

//================================================================================
