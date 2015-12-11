/* BucketData gia hash me kleidi to tid kai value to offset */

#include "DArray.hpp"

class Bucket;
class HashTable;
class BucketDataT
{
    friend class Bucket;
	friend class HashTable;
protected:
    unsigned offset;

 public:
	 BucketDataT(unsigned ioffset);
     unsigned getTid();
     unsigned getOffset();
};
