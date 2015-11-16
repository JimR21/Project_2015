#include "Bucket.hpp"
#include "HashTable.hpp"
#include <iostream>

using namespace std;

HashTable::HashTable()
{
    globalDepth = 1;    // HASHTABLE_SIZE 2 and 2^1 = 2
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
    hashed_key = hashFunction(key);	// pernw to hashed key
	int index = getBucketIndex(hashed_key, globalDepth); // koitaw ta globaldepth deksia bits gia na dw se poio index tha paw
    Bucket* tempBucket = bucketArray[index];	// fere mou to bucket apo auto to index

	cout << getBucketIndex(hashed_key, globalDepth) << endl;

    if(tempBucket->empty == true)	// an to bucket einai empty
    {
		cout << "==============================================" << endl;
		cout << "Bucket is empty. Inserting data with key -> " << key << endl;
		cout << "==============================================" << endl;
        tempBucket->insert(key, data);
    }
    else	// an den einai empty
    {
        if(tempBucket->key == key)
        {
            // #TODO:10 Array of tids
			cout << "Same key case" << endl;
        }
        else
        {
			cout << "==============================================" << endl;
			cout << "Trying to insert different key in full bucket " << endl;
			cout << "==============================================" << endl;
			if (tempBucket->localDepth == globalDepth)	// an einai idio to local tou bucket me to global
			{
				cout << "Global Depth equals Local Depth -> Double array size" << endl;

				unsigned bucket_hashed_key = hashFunction(tempBucket->key);  // redistribute to yparxon key tou bucket

				// oso synexizei na benei sto idio bucket para ton diplasiasmo, sunexise na diplasiazeis
				while (getBucketIndex(bucket_hashed_key, globalDepth) == getBucketIndex(hashed_key, globalDepth)){
					doubleTableSize();
					cout << "DOUBLEING" << endl;
				}

				int index2 = getBucketIndex(bucket_hashed_key, globalDepth); // koitaw ta new globaldepth bits (exoun auksithei)
				cout << "To neo index tou yparxodos bucket: " << index2 << endl;

				cout << "EKEI POU THA PAW = " << index2 << endl;
				cout << "EKEI POU HMOYN = " << index << endl;

				if (index2 != index){ //redistribute to yparxon key tou bucket

					bucketArray[index2] = new Bucket(tempBucket->key, tempBucket->data, globalDepth);

					index = getBucketIndex(hashed_key, globalDepth);
					cout << "To neo index tou neou bucket: " << index << endl;

					bucketArray[index]->localDepth++;
					bucketArray[index]->data = data;
		            bucketArray[index]->key = key;
				}
				else {	// tha minei ekei pou einai
					index = getBucketIndex(hashed_key, globalDepth);
					bucketArray[index] = new Bucket(key, data, globalDepth);
					bucketArray[index2]->localDepth++;
				}
			}
			else if (tempBucket->localDepth < globalDepth)	// split pointers case
			{

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
