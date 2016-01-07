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
// 	unsigned i = 0;
// 	uint32_t start, end, col;
// 	uint64_t val;
// 	string temp = "";
// 	while(key[i] != '-')
// 	{
// 		temp += key[i];
// 		i++;
// 	}
// 	start = stoi(temp);
// 	i++;
//
// 	temp = "";
// 	while(key[i] != '@')
// 	{
// 		temp += key[i];
// 		i++;
// 	}
// 	end = stoi(temp);
// 	i++;
//
// 	temp = "";
// 	while(key[i] >= '0' && key[i] <= '9')
// 	{
// 		temp += key[i];
// 		i++;
// 	}
// 	col = stoi(temp);
//
// 	unsigned op;
// 	char cop = key[i];
// 	switch(cop)
// 	{
// 		case '=' : op = 1; break;
// 		case '!' : op = 2; break;
// 		case '<' : op = 3; break;
// 		case '[' : op = 4; break;
// 		case '>' : op = 5; break;
// 		case ']' : op = 6; break;
// 		default : cout << "Wrong op" << endl;
// 	}
//
// 	i++;
// 	temp = "";
// 	while(i < key.length())
// 	{
// 		temp += key[i];
// 		i++;
// 	}
// 	val = stoul(temp, 0, 10);
//
// 	uint64_t num;
// 	//
// 	// srand(op);
// 	// unsigned lcounter = val % 997;
// 	// for(int j = 0; j < lcounter; j++)
// 	// {
// 	// 	num += rand() % 997;
// 	// }
//
// 	num = (end*2 + start*3 + col*4 + op*5 + val);
//
// 	cout << num << endl;
//
// 	if(num == 474)
// 	{
// 		cout << "ok" << endl;
// 	}
//
//
// 	// int seed = 131;
// 	// unsigned long hash = 0;
// 	// for(unsigned i = 0; i < key.length(); i++)
// 	// {
// 	// 	hash = (hash * seed) + key[i];
// 	// }
// 	// return hash;
//
// 	return (num * 2654435761 % 4294967296);    // Knuth: hash(i)=i*2654435761 mod 2^32
// }

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

void Val_HashTable::split(uint32_t index, uint32_t depth, BucketVal* newbucket)
{
	BucketVal* oldbucket = Buckets.get(index);
	Val_bdata* tempdata = oldbucket->first;
	Val_bdata* tempprev = NULL;
	uint64_t bhashed_key;
	uint32_t bindex;

	Buckets.set(index, newbucket);
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
	if(oldbucket->first == NULL && oldbucket->counter == 1)
	{
		cout << "Wrong" << endl;
	}
}

void Val_HashTable::insert(std::string key)
{
	// cout << "Array size: "<< size << endl;
	uint64_t hashed_key;
	hashed_key = hashFunction(key);
	uint32_t index = getBucketIndex(hashed_key, globalDepth); // koitaw ta globalDepth deksia bits gia na dw se poio index tha paw
	uint32_t bindex;
    BucketVal* tempBucket = Buckets.get(index);

	cout <<"String: " << key << " | HKey: " << hashed_key << " | Index: " << index << endl;

	if(tempBucket->empty == true)
    {
        tempBucket->insert(key);

		// DEBUG
		inserts++;
    }
	else
	{
		if(!datacheck(tempBucket))
		{
			cout << "WRONG" << endl;
		}
		Val_bdata* tempData = tempBucket->keySearch(key);
		if(tempData != NULL)
		{
			tempData->insert();
		}
		else
		{
			if(tempBucket->counter < BUCKET_OVERFLOW)
			{
				tempBucket->insert(key);
			}
			else
			{
				// tsekaroume an sto current global ginetai split se kapoio apo ta val_bdata alliws to auksanoume
				while(!splitcheck(index, globalDepth))
				{
					doubleTableSize();
					index = getBucketIndex(hashed_key, globalDepth);
				}

				// bindex einai to index me diaforetiko globalDepth bit
				unsigned i = pow(2, globalDepth-1);
				if((index & i) == 0)
					bindex = index + i;
				else
					bindex = index - i;

				if(tempBucket->localDepth == globalDepth -1)                        // Simple split otan uparxoun 2 pointers sto bucket
	            {
					split(index, globalDepth, new BucketVal(key, globalDepth));
					Buckets.get(bindex)->localDepth++;
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
	                BucketVal* tempBucketnew = new BucketVal(key, local);
					// split val_bdata
					split(index, globalDepth, tempBucketnew);

	                Buckets.get(bindex)->localDepth++;
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

int Val_HashTable::getbdata(std::string key)
{
	return 0;
}

bool Val_HashTable::datacheck(BucketVal* bucket)
{
	Val_bdata* tempdata = bucket->first;
	int i = 0;
	do
	{
		tempdata = tempdata->next;
		i++;
	} while(tempdata != NULL);
	if(i < bucket->counter)
		return false;
	return true;
}
