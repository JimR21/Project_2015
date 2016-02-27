

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

	// initialize bitset
	for (int i = 0; i < size; i++)
		bitset_array[i] = 0;
}

Bitset::Bitset(const Bitset &bit)
{
	size = bit.size;
	bitset_array = (char*)malloc(size);

	for(int i = 0; i < size; i++)
		bitset_array[i] = bit.bitset_array[i];
}

Bitset::~Bitset() {
	free(bitset_array);
}

void Bitset::setBitsetValue(unsigned index){
	unsigned bit_index = index / CHAR_BIT;		// which char
	unsigned bit_number = index % CHAR_BIT;		// which bit of this char

	bitset_array[bit_index] = bitset_array[bit_index] | (1 << (7 - bit_number));	// update !!
}

unsigned Bitset::getBitsetValue(unsigned index){
	unsigned bit_index = index / CHAR_BIT;		// which char
	unsigned bit_number = index % CHAR_BIT;		// which bit of this char

	if (bitset_array[bit_index] & (1 << bit_number))
		return 1;
	else
		return 0;
}

// FOR TESTING
void Bitset::printBitset(char c){
	int i;

    for (i = 7; i >= 0; --i)
        putchar( (c & (1 << i)) ? '1' : '0' );

    putchar('\n');
}

void Bitset::validateBitset(char *array, int isize){
	unsigned c = isize/8+1;
	bitset_array = (char*)malloc(c);	//1char - 8 bits

	size = isize/8+1;	// keep bitset size

	// initialize bitset
	for (unsigned i = 0; i < c; i++)
		bitset_array[i] = 0;

	bitset_array = array;	// update bitset

}

bool Bitset::validateBitset()
{
	for (int i = 0; i < size; i++)
		if (bitset_array[i] != 0)
			return true;

	return false;
}

void Bitset::addBitset(Bitset * bit)
{
	char* bit_array = bit->getBitsetArray();
	for (int i = 0; i < size; i++)
		bitset_array[i] = bitset_array[i] & bit_array[i];
}

char* Bitset::getBitsetArray(){
	return bitset_array;
}

int Bitset::getSize(){
	return size;
}
