#include "Bucket.hpp"
#include "HashTable.hpp"

extern unsigned primes[25] =
{
    193,389,769,1543,3079,6151,12289,24593,
    49157,98317,196613,393241,786433,1572869,3145739,
    6291469,12582917,25165843,50331653,100663319,
    201326611,402653189,805306457,1610612741
};

using namespace std;

HashTable::HashTable()
{
    globalDepth = 7;    // HASHTABLE_SIZE 97 and 2^7 = 128
    prime_it = 0;       // Initialize prime iterator
    size = HASHTABLE_SIZE;

    // Initiate bucketArray
    // TODO: Change to DArray
    for(unsigned i = 0; i < HASHTABLE_SIZE; i++)
    {
        bucketArray.push_back(new Bucket());
    }

}

HashTable::~HashTable()
{
    // Delete bucketArray
    // TODO: Change to DArray
    // TODO: Delete

    /*for(std::vector<Bucket*>::iterator it = bucketArray.begin(); it != bucketArray.end(); ++it)
    {
        delete *it;
    }*/
}

int HashTable::hashFunction(unsigned int key)
{
    key =  key * 2654435761 % 4294967296;    // Knuth: hash(i)=i*2654435761 mod 2^32
    return (key % (size));
}

void HashTable::doubleTableSize()
{
    globalDepth++;
    unsigned new_size = primes[prime_it];
    for(unsigned i = size; i < new_size; i++)
    {
        bucketArray.push_back(bucketArray[i-size]);
    }
    prime_it++;
    size = new_size;
}

int HashTable::insert(unsigned int key, int data)
{
    unsigned hashed_key;
    hashed_key = hashFunction(key);
    Bucket* tempBucket = bucketArray[hashed_key];
    if(tempBucket->empty == true)
    {
        tempBucket->insert(key, data);
    }
    else
    {
        if(tempBucket->key == key)
        {
            // TODO: Array of tids
        }
        else
        {
            unsigned bhashed_key = hashFunction(tempBucket->key);  // Bucket hashed key
            while(bhashed_key == hashed_key)
            {
                doubleTableSize();
                bhashed_key = hashFunction(tempBucket->key);
                hashed_key = hashFunction(key);
            }
            if(bhashed_key != hashed_key)
            {
                bucketArray[hashed_key] = new Bucket();     // TODO: New constructor
                bucketArray[hashed_key]->insert(key, data);
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

int HashTable::get(int key)
{
    unsigned hashed_key;
    hashed_key = hashFunction(key);
    Bucket* tempBucket = bucketArray[hashed_key];
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
