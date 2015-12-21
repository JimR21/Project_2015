#ifndef BUCKET_HPP
#define BUCKET_HPP

#include "BucketData.hpp"

class HashTable;

class Bucket
{
    friend class Gen_HashTable;
    friend class Key_HashTable;
    friend class Tid_HashTable;
    friend class Val_HashTable;
private:


protected:
    unsigned int key;
    unsigned int localDepth;
    bool empty;

    // BucketData is used in key(c0) Hashtable
    BucketData* first;
    BucketData* last;

    // tid_offset is used in tid HashTable
    int tid_offset;

    Bucket();

    Bucket(unsigned ilocalDepth);

    Bucket(unsigned ikey, unsigned tid, unsigned offset, unsigned ilocalDepth);     // Key HashTable

    Bucket(unsigned ikey, int offset, unsigned ilocalDepth);     // Tid HashTable

    ~Bucket();

    void addBucketData(unsigned tid, unsigned offset);



public:

    void insert(unsigned key, unsigned tid, unsigned offset);   // Key HashTable insert

    void insert(unsigned key, unsigned offset);     // Tid HasthTable insert

    BucketData* getdataLast();
};

#endif
