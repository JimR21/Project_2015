#ifndef BUCKET_HPP
#define BUCKET_HPP

#include "BucketData.hpp"

class HashTable;

class Bucket
{
    friend class HashTable;
    
protected:
    unsigned int key;
    unsigned int localDepth;
    bool empty;
    DArray<BucketData*> data;

    Bucket(unsigned ilocalDepth);

    Bucket(unsigned ikey, BucketData* bdata, unsigned ilocalDepth);

public:
    Bucket();

    ~Bucket();

    void insert(unsigned int key,  BucketData*  data);

    BucketData* getdataLast();
};

#endif
