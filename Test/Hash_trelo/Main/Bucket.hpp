#ifndef BUCKET_HPP
#define BUCKET_HPP

#include "BucketData.hpp"

class HashTable;

class Bucket
{
    friend class HashTable;
private:


protected:
    unsigned int key;
    unsigned int localDepth;
    bool empty;
    DArray<BucketData*> data;

    Bucket();

    Bucket(unsigned ikey, BucketData* bdata, unsigned ilocalDepth);

    ~Bucket();

public:

    void insert(unsigned int key,  BucketData*  data);

};

#endif
