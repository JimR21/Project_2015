#include "DArray.hpp"

class Bucket;
class HashTable;
class BucketData
{
    friend class Bucket;
	friend class HashTable;
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
