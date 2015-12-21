#include "Bucket.hpp"
#include "BucketVal.hpp"
#include "Val_HashTable.hpp"
#include <math.h>

using namespace std;

Val_HashTable::Val_HashTable()
{
	// Constructor gia to val hash me ta new buckets
	for(unsigned i = 0; i < HASHTABLE_SIZE; i++)
        Buckets.push_back(new BucketVal());
}

// int Val_HashTable::hashFunction(const string& key)
// {
// 	int seed = 131;
// 	unsigned long hash = 0;
// 	for(unsigned i = 0; i < key.length(); i++)
// 	{
// 		hash = (hash * seed) + key[i];
// 	}
// 	return hash;
// }
void Val_HashTable::doubleTableSize()
{
    globalDepth++;
    Buckets.specialDouble();

    size *= 2;
    maxLocalCounter.push_back(0);        // Otan diplasiazetai o index sigoura de tha yparxei kanena bucket me localDepth = globalDepth

}


void Val_HashTable::insert(std::string key)
{
	// cout << "Array size: "<< size << endl;
	unsigned hashed_key;
	hashed_key = hashFunction(key);
	int index = getBucketIndex(hashed_key, globalDepth); // koitaw ta globalDepth deksia bits gia na dw se poio index tha paw
    BucketVal* tempBucket = Buckets.get(index);

	if(tempBucket->empty == true)
    {
        tempBucket->insert(key);
    }
	else{
		if(key.compare(tempBucket->vkey) == 0)	// edw ama erxetai to idio tha prepei na kanw update ton counter
		{										// kai to val pou anikei auto to predicate

		}
		else{
			unsigned bhashed_key = hashFunction(tempBucket->vkey);  // Bucket hashed key
            while(getBucketIndex(bhashed_key, globalDepth) == getBucketIndex(hashed_key, globalDepth))
            {
                Val_HashTable::doubleTableSize();
            }
            int index2 = getBucketIndex(bhashed_key, globalDepth);
            index = getBucketIndex(hashed_key, globalDepth);

			if(tempBucket->localDepth == globalDepth -1)                        // Simple split otan uparxoun 2 pointers sto bucket
            {
                Buckets.set(index, new BucketVal(key, globalDepth));
                Buckets.get(index2)->localDepth++;
                maxLocalCounter.set(maxLocalCounter.size()-1, maxLocalCounter.get(maxLocalCounter.size()-1)+2);
            }
			else           // Split otan uparxoyn perissoteroi pointers sto bucket
            {
                unsigned local = Buckets.get(index2)->localDepth;
                while(getBucketIndex(bhashed_key, local) == getBucketIndex(hashed_key, local))  // Oso to localDepth den einai arketo gia na diaxwristoun ta 2 kleidia, auksanetai kai diaxwrizontai ta buckets
                {
                    if(getBucketIndex(bhashed_key, local+1) != getBucketIndex(hashed_key, local+1)) // An sto localDepth+1 diaxwrizetai vges apo loop
                        break;
                    local++;
                    Buckets.get(index2)->localDepth++;
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
                BucketVal* tempBucketnew = new BucketVal(key, local);
                Buckets.set(index, tempBucketnew);
                Buckets.get(index2)->localDepth++;
                unsigned toindex = getBucketIndex(hashed_key, local);        // Ypologismos tou bucket index me to neo local depth
                unsigned dist = pow(2, local);                               // H apostash poy tha exei to bucket index me ton epomeno deikth poy tha deiksei sto new bucket
                for(unsigned i = toindex; i < size; i+=dist)                 // Metafora deiktwn tou indexTable sto tempBucketnew
                {
                    Buckets.set(i, tempBucketnew);
                }
                if(local == globalDepth)   // An to local iso me global tote ta 2 bucket exoun localDepth = globalDepth kai to maxLocalCounter auksanetai kata 2
                    maxLocalCounter.set(maxLocalCounter.size()-1, maxLocalCounter.get(maxLocalCounter.size()-1)+2);
            }
		}
	}
	// cout << "VGIKA INSERT" << endl;
}

int Val_HashTable::getbdata(std::string key)
{
	return 0;
}
