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
    bool validated;     // True if validated
    Val_bdata* next;


 public:
	 Val_bdata(std::string key, unsigned range);
	 void insert();	// pros to paron counter ++ mono meta tha vazei kai to val
     void setBitsetValue(int index, char *array);	// set Bitset index to 1
	 int getBitsetValue(int index, char *array);	// get the value of this index

	 void printBitset(char c); // print the array

	 char* validate(DArray<bool> *array);
};

#endif
