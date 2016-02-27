#include "BucketVal.hpp"
#include "Val_HashTable.hpp"
#include <math.h>
#include <string>

using namespace std;

Val_HashTable::Val_HashTable()
{
	globalDepth = STARTING_GLOBAL_DEAPTH;    // 2^7 = 128
    size = HASHTABLE_SIZE;
    maxLocalCounter.push_back(HASHTABLE_SIZE);

    // DEBUG
    inserts = 0;

	// Constructor gia to val hash me ta new buckets
	for(unsigned i = 0; i < HASHTABLE_SIZE; i++)
        Buckets.push_back(new BucketVal());
}

Val_HashTable::~Val_HashTable()
{
	// Delete bucketArray
	Val_bdata* data;
	Val_bdata* nextdata;
	BucketVal* bucket;
    for(unsigned i = 0; i < size; i++)
    {
        if(Buckets.get(i) != NULL)
        {
			bucket = Buckets.get(i);
            unsigned p = pow(2, bucket->localDepth);
			// if(bucket->empty == false)
			// {
			// 	data = bucket->first;
			// 	nextdata = NULL;
			// 	for(int j = 0; j < bucket->counter; j++)
			// 	{
			// 		nextdata = data->next;
			// 		delete data;
			// 		data = nextdata;
			// 	}
			// }
            delete bucket;
            for(unsigned j = i; j < size; j+=p)
                Buckets.set(j, NULL);
        }
    }
}

// int Val_HashTable::hashFunction(const string& key)
// {
//
// }

uint32_t Val_HashTable::getBucketIndex(uint64_t hash, int depth)
{
	return hash & ( (1 << depth) - 1);
}

void Val_HashTable::doubleTableSize()
{
    globalDepth++;
    Buckets.specialDouble();

    size *= 2;
    maxLocalCounter.push_back(0);        // Otan diplasiazetai o index sigoura de tha yparxei kanena bucket me localDepth = globalDepth

}

bool Val_HashTable::splitcheck(uint32_t index, uint32_t depth)
{
	BucketVal* bucket = Buckets.get(index);
	Val_bdata* tempdata = bucket->first;
	uint64_t bhashed_key;
	uint32_t bindex;
	do
	{
		bhashed_key = hashFunction(tempdata->key);
		bindex = getBucketIndex(bhashed_key, depth);
		if(bindex != index)
		{
			return true;
		}
		tempdata = tempdata->next;
	} while(tempdata != NULL);
	return false;
}

void Val_HashTable::split(uint32_t index, uint32_t depth, BucketVal* newbucket, string key, unsigned range)
{
	BucketVal* oldbucket = Buckets.get(index);
	Val_bdata* tempdata = oldbucket->first;
	Val_bdata* tempprev = NULL;
	uint64_t bhashed_key;
	uint32_t bindex;

	do
	{	// gia BUCKET_OVERFLOW epanalipseis
		bhashed_key = hashFunction(tempdata->key);
		bindex = getBucketIndex(bhashed_key, depth);
		if(bindex == index)
		{
			// diagrafh apo to palio
			if(tempprev == NULL)	// Periptwsh pou einai tempdata = first
				oldbucket->first = tempdata->next;
			else
				tempprev->next = tempdata->next;
			oldbucket->counter--;
			if(oldbucket->counter == 0)
			{
				oldbucket->first = NULL;
				oldbucket->last = NULL;
				oldbucket->empty = true;
			}
			if(tempdata->next == NULL)
			{
				if(tempprev == NULL)	// Periptwsh pou einai tempdata = first
					oldbucket->last = oldbucket->first;
				else
					oldbucket->last = tempprev;
			}
			// insert sto neo bucket

			tempdata->next = NULL;
			newbucket->insert(tempdata);
			if(tempprev == NULL)
				tempdata = oldbucket->first;
			else
				tempdata = tempprev->next;		// temprev paramenei idio
		}
		else
		{
			tempprev = tempdata;
			tempdata = tempdata->next;
		}
	}while(tempdata != NULL);

	newbucket->insert(key,range);

	oldbucket->localDepth++;

	Buckets.set(index, newbucket);

	if(oldbucket->first == NULL && oldbucket->counter == 1)
	{
		cout << "Wrong" << endl;
	}
}

void Val_HashTable::insert(std::string key, unsigned range)
{
	// cout << "Array size: "<< size << endl;
	uint64_t hashed_key;
	hashed_key = hashFunction(key);
	uint32_t index = getBucketIndex(hashed_key, globalDepth); // koitaw ta globalDepth deksia bits gia na dw se poio index tha paw
    BucketVal* tempBucket = Buckets.get(index);

	//cout <<"String: " << key << " | HKey: " << hashed_key << " | Index: " << index << endl;

	if(tempBucket->empty == true)
    {
        tempBucket->insert(key,range);

		// DEBUG
		inserts++;
    }
	else
	{
		Val_bdata* tempData = tempBucket->keySearch(key);
		if(tempData != NULL)
		{
			tempData->insert();
		}
		else
		{
			if(tempBucket->counter < BUCKET_OVERFLOW)
			{
				tempBucket->insert(key,range);
			}
			else
			{
				// tsekaroume an sto current global ginetai split se kapoio apo ta val_bdata alliws to auksanoume
				while(!splitcheck(index, globalDepth))
				{
					doubleTableSize();
					index = getBucketIndex(hashed_key, globalDepth);
				}

				if(tempBucket->localDepth == globalDepth -1)  // Simple split otan uparxoun 2 pointers sto bucket
	            {
					split(index, globalDepth, new BucketVal(globalDepth), key,range);
					maxLocalCounter.set(maxLocalCounter.size()-1, maxLocalCounter.get(maxLocalCounter.size()-1)+2);
				}
				else           // Split otan uparxoyn perissoteroi pointers sto bucket
	            {
	                unsigned local = tempBucket->localDepth;
	                while(!splitcheck(getBucketIndex(hashed_key, local), local))  // Oso to localDepth den einai arketo gia na diaxwristoun ta 2 kleidia, auksanetai kai diaxwrizontai ta buckets
	                {
	                    if(splitcheck(getBucketIndex(hashed_key, local+1), local+1)) // An sto localDepth+1 diaxwrizetai vges apo loop
	                        break;
	                    local++;
	                    tempBucket->localDepth++;
	                    BucketVal* newBucket = new BucketVal(local);                                        // Neo empty bucket

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
	                        Buckets.set(i, newBucket);
	                    }
	                }

	                local++;
	                BucketVal* tempBucketnew = new BucketVal(local);
					// split val_bdata
					split(getBucketIndex(hashed_key, local), local, tempBucketnew, key, range);

	                unsigned toindex = getBucketIndex(hashed_key, local);        // Ypologismos tou bucket index me to neo local depth
	                unsigned dist = pow(2, local);                               // H apostash poy tha exei to bucket index me ton epomeno deikth poy tha deiksei sto new bucket
	                for(unsigned i = toindex; i < size; i+=dist)                 // Metafora deiktwn tou indexTable sto tempBucketnew
	                {
	                    Buckets.set(i, tempBucketnew);
	                }
	                if(local == globalDepth)   // An to local iso me global tote ta 2 bucket exoun localDepth = globalDepth kai to maxLocalCounter auksanetai kata 2
	                    maxLocalCounter.set(maxLocalCounter.size()-1, maxLocalCounter.get(maxLocalCounter.size()-1)+2);
	            }

				// DEBUG
				inserts++;
			}
		}
		// cout << "VGIKA INSERT" << endl;
	}
}

Bitset* Val_HashTable::getbdata(std::string key, int *counter)
{
	unsigned hashed_key;
    hashed_key = hashFunction(key);
	int index = getBucketIndex(hashed_key, globalDepth); // koitaw ta globaldepth deksia bits gia na dw se poio index tha paw
    BucketVal* bucket = Buckets.get(index);
	Val_bdata* tempdata = bucket->first;

    if(bucket->empty == false)
	{
		do
		{
			if(key.compare(tempdata->key) == 0)
			{
				tempdata->counter--;
				*counter = tempdata->counter;
				if (tempdata->validated == true)
					return new Bitset(*tempdata->bitset);
				else
					return NULL;
			}
			tempdata = tempdata->next;
		} while(tempdata != NULL);
	}
    //cout << "getbdata: Key not found" << endl;
	*counter = -1;
    return NULL;
}

int Val_HashTable::deleteKey(string key)
{
	unsigned hashed_key;
    hashed_key = hashFunction(key);
    unsigned idx = getBucketIndex(hashed_key, globalDepth);
    BucketVal* bucket = Buckets.get(idx);

    // TESTING IF
    if((bucket->empty == false) && (popKey(bucket, key)))
    {
		if(bucket->counter == 0)
		{
	        if(bucket->localDepth < globalDepth)            // An einai local < global tote kane empty to bucket
	        {
	            bucket->empty = true;
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

	            BucketVal* mergeBucket = Buckets.get(idx2);
	            mergeBucket->localDepth--;

	            // Change the pointer to mergeBucket
	            Buckets.set(idx, mergeBucket);
	            delete bucket;

	            maxLocalCounter.set(maxLocalCounter.size()-1, maxLocalCounter.get(maxLocalCounter.size()-1)-2);
	            if(maxLocalCounter.get(maxLocalCounter.size()-1) == 0)
	                halveTableSize();
	        }
		}
		return 1;
    }
    return 0;
}

bool Val_HashTable::popKey(BucketVal* bucket, string key)
{
	if(bucket->counter == 0)
		return false;
	Val_bdata* tempdata = bucket->first;
	Val_bdata* prevdata = NULL;
	do
	{
		if(key.compare(tempdata->key) == 0)
		{
			if(tempdata == bucket->first)
			{
				if(bucket->counter > 1)
					bucket->first = tempdata->next;
				else
				{
					bucket->first = NULL;
					bucket->last = NULL;
				}
			}
			else if(tempdata == bucket->last)
			{
				prevdata->next = NULL;
				bucket->last = prevdata;
			}
			else
			{
				prevdata->next = tempdata->next;
			}
			bucket->counter --;
			delete tempdata;
			return true;
		}
		prevdata = tempdata;
		tempdata = tempdata->next;
	} while(tempdata != NULL);
	return false;
}

void Val_HashTable::halveTableSize()
{
    globalDepth--;
    size /= 2;
    for(unsigned i = 0; i < size; i++)
        Buckets.popLast();
    maxLocalCounter.popLast();
}

unsigned Val_HashTable::getsize()
{	return size; }

void Val_HashTable::UpdateValData(std::string key, Bitset* bit){
	unsigned hashed_key;
    hashed_key = hashFunction(key);
	int index = getBucketIndex(hashed_key, globalDepth); // koitaw ta globaldepth deksia bits gia na dw se poio index tha paw

	BucketVal* bucket = Buckets.get(index);
	Val_bdata* tempdata = bucket->first;

	if(bucket->empty == false)
	{
		do
		{
			if(key.compare(tempdata->key) == 0)
			{
				tempdata->validate(bit);
				return;
			}
			tempdata = tempdata->next;
		} while(tempdata != NULL);
	}

	return;
}
