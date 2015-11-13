#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP
#include <vector>
#include <bitset>


#define HASHTABLE_SIZE 97

class Bucket;

class HashTable
{
    friend class Bucket;
private:
    // TODO deikths se arxh journal

    unsigned prime_it;  // Prime iterator

    unsigned size;

    std::vector<Bucket*> bucketArray;  // Pointers to Buckets

    // Returns hashed number
    int hashFunction(unsigned int key);

    //Doubles table to a prime
    void doubleTableSize();

protected:
    unsigned int globalDepth;


public:

    HashTable();

    ~HashTable();

    int insert(unsigned int key, int data);

    // Returns Bucket address
    int get(int key);

    unsigned getsize();

};


#endif
