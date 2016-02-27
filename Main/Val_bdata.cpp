#include "Val_bdata.hpp"
#include <malloc.h>
#include <limits.h>

Val_bdata::Val_bdata(std::string ikey, unsigned range)
{
	key = ikey;
	counter = 1;
	//vals = new DArray<Val_listbucket*>(2);
	bitset = NULL;

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


void Val_bdata::validate(Bitset* bit){

	validated = true;	// to predicate se auto to range einai pleon validated

	// update to bitset
	bitset = new Bitset(*bit);

	return;
}
