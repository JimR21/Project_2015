#ifndef VAL_BDATA_HPP
#define VAL_BDATA_HPP

#include "DArray.hpp"
#include "Val_listbucket.hpp"

class Bucket;
class Val_HashTable;

class Val_bdata
{
    friend class Bucket;
	friend class Val_HashTable;
	friend class BucketVal;
protected:
    std::string key;
    char* bitset;
	DArray<Val_listbucket*> *vals;	// pointers to vals
    int counter;     // counts pointers from vals
    Val_bdata* next;


 public:
	 Val_bdata();
	 Val_bdata(std::string key);
	 void insert();	// pros to paron counter ++ mono meta tha vazei kai to val
};

#endif
