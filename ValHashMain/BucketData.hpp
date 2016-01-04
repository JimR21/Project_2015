#ifndef BUCKETDATA_HPP
#define BUCKETDATA_HPP

#include "DArray.hpp"

class Bucket;
class Key_HashTable;

class BucketData
{
    friend class Bucket;
	friend class Key_HashTable;
protected:
    unsigned tid;
    unsigned offsets[2];
    bool second_of;     // True if there is a second offset
    BucketData* next;

 public:
	 BucketData(unsigned t, unsigned offset);
     unsigned getTid();
     unsigned * getOffsets();
     unsigned getOffsetLast();
};


#endif
