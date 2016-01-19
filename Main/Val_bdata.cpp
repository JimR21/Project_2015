#include "Val_bdata.hpp"
#include <malloc.h>
#include <limits.h>

Val_bdata::Val_bdata(std::string ikey, unsigned range)
{
	key = ikey;
	counter = 1;
	vals = new DArray<Val_listbucket*>(2);

	// unsigned c = range/8+1;
	// bitset = (char*)malloc(c);	//1char - 8 bits
	//
	// for (unsigned i = 0; i < c; i++)
	// 	bitset[i] = 0;

	validated = false;
	next = NULL;
}

void Val_bdata::insert(){
	//TODO insert to validation
	counter++;
}

void Val_bdata::setBitsetValue(int index, char *array){
	int bit_index = index / CHAR_BIT;		// which char
	int bit_number = index % CHAR_BIT;		// which bit of this char

	array[bit_index] = array[bit_index] | (1 << (7 - bit_number));	// update !!
}

int Val_bdata::getBitsetValue(int index, char *array){
	int bit_index = index / CHAR_BIT;		// which char
	int bit_number = index % CHAR_BIT;		// which bit of this char

	if (array[bit_index] & (1 << bit_number))
		return 1;
	else
		return 0;
}

void Val_bdata::printBitset(char c){
	int i;

    for (i = 7; i >= 0; --i)
        putchar( (c & (1 << i)) ? '1' : '0' );

    putchar('\n');
}

char* Val_bdata::validate(DArray<bool> *array, int size){

	validated = true;	// to predicate se auto to range einai pleon validated

	unsigned c = size/8+1;
	bitset = (char*)malloc(c);	//1char - 8 bits

	for (unsigned i = 0; i < c; i++)
		bitset[i] = 0;

	for (int i = 0; i < c; i ++)
		if (array->get(i) == true)		// an conflict
			setBitsetValue(i, bitset);	// update auto to index
	// printBitset(*bitset);
	return bitset;
}
