

#include "bitset.hpp"
#include <malloc.h>
#include <limits.h>

Bitset::Bitset(){
	bitset_array = NULL;
	size = -1;
}

Bitset::Bitset(unsigned isize) : size(isize)
{
	bitset_array = (char*)malloc(isize);
}

Bitset::Bitset(const Bitset &bit)
{
	size = bit.size;
	bitset_array = (char*)malloc(size);
	for(unsigned i = 0; i < size; i++)
	{
		bitset_array[i] = bit.bitset_array[i];
	}
}

Bitset::~Bitset()
{
	free(bitset_array);
}

void Bitset::setBitsetValue(int index, char *array){
	int bit_index = index / CHAR_BIT;		// which char
	int bit_number = index % CHAR_BIT;		// which bit of this char

	array[bit_index] = array[bit_index] | (1 << (7 - bit_number));	// update !!
}

int Bitset::getBitsetValue(int index, char *array){
	int bit_index = index / CHAR_BIT;		// which char
	int bit_number = index % CHAR_BIT;		// which bit of this char

	if (array[bit_index] & (1 << bit_number))
		return 1;
	else
		return 0;
}

void Bitset::setBitsetArray(int index)
{
	int bit_index = index / CHAR_BIT;		// which char
	int bit_number = index % CHAR_BIT;		// which bit of this char

	bitset_array[bit_index] = bitset_array[bit_index] | (1 << (7 - bit_number));	// update !!
}

int Bitset::getBitsetArray(int index)
{
	int bit_index = index / CHAR_BIT;		// which char
	int bit_number = index % CHAR_BIT;		// which bit of this char

	if (bitset_array[bit_index] & (1 << bit_number))
		return 1;
	else
		return 0;
}

void Bitset::printBitset(char c){
	int i;

    for (i = 7; i >= 0; --i)
        putchar( (c & (1 << i)) ? '1' : '0' );

    putchar('\n');
}

void Bitset::validateBitset(DArray<bool> *array, int isize){
	unsigned c = isize/8+1;
	bitset_array = (char*)malloc(c);	//1char - 8 bits

	size = isize/8+1;	// keep bitset size

	// initialize bitset
	for (unsigned i = 0; i < c; i++)
		bitset_array[i] = 0;

	// update bitset
	for (int i = 0; i < isize; i ++)
		if (array->get(i) == true)				// an conflict
			setBitsetValue(i, bitset_array);	// update auto to index

}


char* Bitset::getBitsetArray(){
	return bitset_array;
}

int Bitset::getSize(){
	return size;
}
