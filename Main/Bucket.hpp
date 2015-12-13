#ifndef BUCKET_HPP
#define BUCKET_HPP

#include "BucketData.hpp"

class HashTable;

class Bucket
{
    friend class Gen_HashTable;
    friend class Key_HashTable;
private:


protected:
    unsigned int key;
    unsigned int localDepth;
    bool empty;

    // BucketData is used in key(c0) Hashtable
    BucketData* first;
    BucketData* last;

    // tid_offset is used in tid HashTable
    unsigned tid_offset;


    Bucket();

    Bucket(unsigned ilocalDepth);

    Bucket(unsigned ikey, unsigned tid, unsigned offset, unsigned ilocalDepth);

    ~Bucket();

    void addBucketData(unsigned tid, unsigned offset);



public:

    void insert(unsigned key, unsigned tid, unsigned offset);

    BucketData* getdataLast();
};

#endif
