#include "Val_bdata.hpp"
#include <malloc.h>
#include <limits.h>

Val_bdata::Val_bdata(std::string ikey, unsigned range)
{
	key = ikey;
	counter = 1;
	//vals = new DArray<Val_listbucket*>(2);
	bitset = new Bitset();

	validated = false;
	next = NULL;
}

Val_bdata::~Val_bdata()
{
	delete bitset;
}

void Val_bdata::insert(){
	//TODO insert to validation
	counter++;
}


Bitset* Val_bdata::validate(DArray<bool> *array, int size){

	validated = true;	// to predicate se auto to range einai pleon validated

	// update to bitset
	bitset->validateBitset(array, size);

	return bitset;
}
