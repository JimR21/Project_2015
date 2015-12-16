#include "Val_bdata.hpp"
#include <malloc.h>

Val_bdata::Val_bdata(){
	counter = 0;
	vals = new DArray<Val_listbucket*>();
	bitset = (char*)malloc(1);	//1char - 8 bits
}

void Val_bdata::insert(){
	//TODO insert to validation
	counter++;
}
