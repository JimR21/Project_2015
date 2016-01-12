#include "Val_bdata.hpp"
#include <malloc.h>
#include <limits.h>

// Val_bdata::Val_bdata(){
// 	counter = 0;
// 	vals = new DArray<Val_listbucket*>(2);
// 	bitset = (char*)malloc(1);	//1char - 8 bits
// 	validated = false;
// 	next = NULL;
// }

Val_bdata::Val_bdata(std::string ikey, unsigned range)
{
	key = ikey;
	counter = 1;
	vals = new DArray<Val_listbucket*>(2);

	unsigned c = range/8+1;
	bitset = (char*)malloc(c);	//1char - 8 bits

	for (unsigned i = 0; i < c; i++)
	{
		bitset[i] = 0;
	}

	validated = false;
	next = NULL;
}

void Val_bdata::insert(){
	//TODO insert to validation
	counter++;
}

void setBitsetValue(int index, char *array){
	int bit_index = index / CHAR_BIT;		// which char
	int bit_number = index % CHAR_BIT;		// which bit of this char

	array[bit_index] = array[bit_index] | (1 << (7 - bit_number));	// update !!
}

int getBitsetValue(int index, char *array){
	int bit_index = index / CHAR_BIT;		// which char
	int bit_number = index % CHAR_BIT;		// which bit of this char

	if (array[bit_index] & (1 << bit_number))
		return 1;
	else
		return 0;
}

void printBitset(char c){
	int i;

    for (i = 7; i >= 0; --i)
        putchar( (c & (1 << i)) ? '1' : '0' );

    putchar('\n');
}
