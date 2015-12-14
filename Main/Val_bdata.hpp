#ifndef VAL_BDATA_HPP
#define VAL_BDATA_HPP

#include "DArray.hpp"

class Bucket;
class Val_HashTable;

class Val_bdata
{
    friend class Bucket;
	friend class Val_HashTable;
protected:
    // bitset
    int counter     // counts pointers from vals
;

 public:

};

#endif
