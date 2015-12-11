#ifndef BUCKET_HPP
#define BUCKET_HPP

#include "BucketData.hpp"
#include "BucketDataT.hpp"

class HashTable;

class Bucket
{
    friend class HashTable;
private:


protected:
    unsigned int key;
    unsigned int localDepth;
    bool empty;
    BucketData* first;
    BucketData* last;
	BucketDataT* tfirst;

    Bucket();

    Bucket(unsigned ilocalDepth);

    Bucket(unsigned ikey, unsigned tid, unsigned offset, unsigned ilocalDepth);

    ~Bucket();

	void addBucketData(unsigned tid, unsigned offset);


public:

	void insert(unsigned key, unsigned tid, unsigned offset);

    void tidInsert(unsigned key, unsigned offset);

    BucketData* getdataLast();

	BucketDataT* getData();
};

#endif
