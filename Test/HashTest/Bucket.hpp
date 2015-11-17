#ifndef BUCKET_HPP
#define BUCKET_HPP
#include "DArray.hpp"


class HashTable;

class BucketData
{
    unsigned tid;
    DArray<unsigned> rangeArray;

 public:
     unsigned getTid();

     DArray<unsigned> getrangeArray();
};


class Bucket
{
    friend class HashTable;
private:


protected:
    unsigned int key;
    unsigned int localDepth;
    bool empty;
    DArray<BucketData> data;

    Bucket();

    Bucket(unsigned ikey, int idata, unsigned ilocalDepth);

    ~Bucket();

public:

    void insert(unsigned int key, int data);

};

#endif
