#include "Bucket.hpp"
#include "Key_HashTable.hpp"
#include <math.h>
#include <iostream>

using namespace std;

//=======================================================================================================
void Key_HashTable::insert(unsigned int key, unsigned tid, unsigned offset)
{
    unsigned hashed_key;
    hashed_key = hashFunction(key);
    int index = getBucketIndex(hashed_key, globalDepth); // koitaw ta globalDepth deksia bits gia na dw se poio index tha paw
    Bucket* tempBucket = bucketArray.get(index);

    if(tempBucket->empty == true)
    {
        tempBucket->insert(key, tid, offset);
    }
    else
    {
        // An to key yparxei...
        if(tempBucket->key == key)
        {
            tempBucket->insert(key, tid, offset);
        }
        else
        {
            unsigned bhashed_key = hashFunction(tempBucket->key);  // Bucket hashed key
            while(getBucketIndex(bhashed_key, globalDepth) == getBucketIndex(hashed_key, globalDepth))
            {
                doubleTableSize();
            }
            int index2 = getBucketIndex(bhashed_key, globalDepth);
            index = getBucketIndex(hashed_key, globalDepth);

            if(tempBucket->localDepth == globalDepth -1)                        // Simple split otan uparxoun 2 pointers sto bucket
            {
                bucketArray.set(index, new Bucket(key, tid, offset, globalDepth));
                bucketArray.get(index2)->localDepth++;
                maxLocalCounter.set(maxLocalCounter.size()-1, maxLocalCounter.get(maxLocalCounter.size()-1)+2);
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
                Bucket* tempBucketnew = new Bucket(key, tid, offset, local);
                bucketArray.set(index, tempBucketnew);
                bucketArray.get(index2)->localDepth++;
                unsigned toindex = getBucketIndex(hashed_key, local);        // Ypologismos tou bucket index me to neo local depth
                unsigned dist = pow(2, local);                               // H apostash poy tha exei to bucket index me ton epomeno deikth poy tha deiksei sto new bucket
                for(unsigned i = toindex; i < size; i+=dist)                 // Metafora deiktwn tou indexTable sto tempBucketnew
                {
                    bucketArray.set(i, tempBucketnew);
                }
                if(local == globalDepth)   // An to local iso me global tote ta 2 bucket exoun localDepth = globalDepth kai to maxLocalCounter auksanetai kata 2
                    maxLocalCounter.set(maxLocalCounter.size()-1, maxLocalCounter.get(maxLocalCounter.size()-1)+2);
            }
        }
    }
}
//=======================================================================================================

DArray<uint64_t>* Key_HashTable::getHashRecord(unsigned key, uint64_t start_tid, uint64_t end_tid){
	unsigned hashed_key;
    hashed_key = hashFunction(key);
	int index = getBucketIndex(hashed_key, globalDepth); // koitaw ta globaldepth deksia bits gia na dw se poio index tha paw
    Bucket* tempBucket = bucketArray.get(index);

    DArray<uint64_t>* array = new DArray<uint64_t>();

	if((tempBucket->empty == false) && (tempBucket->key == key))
    {
		BucketData* tempData = tempBucket->first;
        do
        {
            if(tempData->tid > end_tid)
                break;
            if(tempData->tid >= start_tid)
            {
                array->push_back(tempData->offsets[0]);
                if(tempData-> second_of == true)
                    array->push_back(tempData->offsets[1]);
            }
            tempData = tempData->next;
        }while(tempData != NULL);
        if(array->size() == 0)
        {
            delete array;
            return NULL;
        }
        return array;
	}
	else
        // cout << "getHashRecord: Key not found" << endl;
    if(array->size() == 0)
    {
        delete array;
        return NULL;
    }
	return array;
}
//=======================================================================================================
DArray<uint64_t>* Key_HashTable::getHashRecords(unsigned key)
{
    unsigned hashed_key;
    hashed_key = hashFunction(key);
	int index = getBucketIndex(hashed_key, globalDepth); // koitaw ta globaldepth deksia bits gia na dw se poio index tha paw
    Bucket* tempBucket = bucketArray.get(index);

	DArray<uint64_t>* array = new DArray<uint64_t>();

    if((tempBucket->empty == false) && (tempBucket->key == key)){
        BucketData* tempData = tempBucket->first;
        do
        {
            array->push_back(tempData->offsets[0]);
            if(tempData-> second_of == true)
                array->push_back(tempData->offsets[1]);
            tempData = tempData->next;
        }while(tempData != NULL);
        if(array->size() == 0)
        {
            delete array;
            return NULL;
        }
        return array;
	}
    else
        // cout << "getHashRecords: Key not found" << endl;
    if(array->size() == 0)
    {
        delete array;
        return NULL;
    }
	return array;
}
//=======================================================================================================
bool Key_HashTable::existCheck(unsigned key, uint64_t start_tid, uint64_t end_tid){

	unsigned hashed_key = hashFunction(key);
	unsigned idx = getBucketIndex(hashed_key, globalDepth);
    Bucket* tempBucket = bucketArray.get(idx);

	if((tempBucket->empty == false) && (tempBucket->key == key)){
        BucketData* tempData = tempBucket->first;
        do
        {
            if(tempData->tid >= start_tid && tempData->tid <= end_tid)
                return true;
                tempData = tempData->next;
        }while(tempData != NULL);
	}
	return false;
}
//=======================================================================================================
int Key_HashTable::getLastJournalInsert(unsigned key)  // NEW
{
    unsigned hashed_key;
    hashed_key = hashFunction(key);
    unsigned idx = getBucketIndex(hashed_key, globalDepth);
    Bucket* tempBucket = bucketArray.get(idx);

    //  TESTING IF
    if((tempBucket->empty == false) && (tempBucket->key == key))
    {
        BucketData* bdata = tempBucket->last;
        return bdata->getOffsetLast();
    }
    else
        // cout << "Key not found" << endl;
    return -1;
}
//=======================================================================================================
