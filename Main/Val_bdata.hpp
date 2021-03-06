#ifndef VAL_BDATA_HPP
#define VAL_BDATA_HPP

#include "DArray.hpp"
#include "Val_listbucket.hpp"
#include "bitset.hpp"

class Bucket;
class Val_HashTable;

class Val_bdata
{
    friend class Bucket;
	friend class Val_HashTable;
	friend class BucketVal;
protected:
    std::string key;
    Bitset* bitset;
	//DArray<Val_listbucket*> *vals;	// pointers to vals
    int counter;     // counts pointers from vals
    bool validated;     // True if validated
    Val_bdata* next;


 public:
	 Val_bdata(std::string key, unsigned range);
	 ~Val_bdata();

	 void insert();	// pros to paron counter ++ mono meta tha vazei kai to val

	 void printBitset(char c); // print the array

	 void validate(Bitset* bit);
};

#endif
