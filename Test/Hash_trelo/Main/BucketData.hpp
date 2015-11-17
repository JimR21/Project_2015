#include "DArray.hpp"

class Bucket;
class HashTable;
class BucketData
{
    friend class Bucket;
	friend class HashTable;
protected:
    unsigned tid;
    DArray<unsigned> rangeArray;

 public:
	 BucketData(unsigned t, unsigned offset);
     unsigned getTid();
     DArray<unsigned> getrangeArray();
};
