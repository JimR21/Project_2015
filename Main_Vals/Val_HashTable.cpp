#include "Bucket.hpp"
#include "Val_HashTable.hpp"
#include <math.h>

using namespace std;

Val_HashTable::Val_HashTable()
{
	cout << "Val HashTable created" << endl;
}

int Val_HashTable::hashFunction(const string& key)
{
	int seed = 131;
	unsigned long hash = 0;
	for(unsigned i = 0; i < key.length(); i++)
	{
		hash = (hash * seed) + key[i];
	}
	return hash;
}

void Val_HashTable::insert(std::string key)
{
	unsigned hashed_key;
	hashed_key = hashFunction(key);
	int index = getBucketIndex(hashed_key, globalDepth); // koitaw ta globalDepth deksia bits gia na dw se poio index tha paw
    Bucket* tempBucket = bucketArray.get(index);

	// cout << "INDEX: " << index << endl;
	if(tempBucket->empty == true)
    {
		// cout << "EMPTY -> TO VAZW" << endl;
        tempBucket->insert(key);
    }
	else{
		if(key.compare(tempBucket->vkey) == 0)	// edw ama erxetai to idio tha prepei na kanw update ton counter
		{										// kai to val pou anikei auto to predicate
			// cout << "hmm" << endl;
		}
		else{
			// cout << "Gemato bucket" << endl;

			unsigned bhashed_key = hashFunction(tempBucket->vkey);  // Bucket hashed key
            while(getBucketIndex(bhashed_key, globalDepth) == getBucketIndex(hashed_key, globalDepth))
            {
				// cout << "hehe";
                doubleTableSize();
            }
            int index2 = getBucketIndex(bhashed_key, globalDepth);
            index = getBucketIndex(hashed_key, globalDepth);

			if(tempBucket->localDepth == globalDepth -1)                        // Simple split otan uparxoun 2 pointers sto bucket
            {
                bucketArray.set(index, new Bucket(key, globalDepth));
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
                Bucket* tempBucketnew = new Bucket(key, local);
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

int Val_HashTable::getbdata(std::string key)
{
	return 0;
}
unsigned hashed_key;
