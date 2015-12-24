#include "Val_bdata.hpp"
#include <malloc.h>

Val_bdata::Val_bdata(){
	counter = 0;
	vals = new DArray<Val_listbucket*>(2);
	bitset = (char*)malloc(1);	//1char - 8 bits
}

Val_bdata::Val_bdata(std::string ikey)
{
	key = ikey;
	counter = 1;
	vals = new DArray<Val_listbucket*>(2);
	bitset = (char*)malloc(1);	//1char - 8 bits
}

void Val_bdata::insert(){
	//TODO insert to validation
	counter++;
}
