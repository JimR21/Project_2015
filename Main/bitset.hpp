#ifndef BITSET_HPP
#define BITSET_HPP

#include "DArray.hpp"

class Bucket;
class Val_HashTable;


class Bitset {
	friend class Bucket;
	friend class Val_HashTable;
	friend class BucketVal;
protected:
	char *bitset_array;
	int size;

public:
	Bitset();

	~Bitset();

	Bitset(unsigned size);

	Bitset(const Bitset &bit);

	void setBitsetValue(int index, char *array);	// set Bitset index to 1
	int getBitsetValue(int index, char *array);		// get the value of this index

	void setBitsetArray(int index);	// set Bitset index to 1
	int getBitsetArray(int index);		// get the value of this index

	void printBitset(char c); // print the array

	void validateBitset(DArray<bool> *array, int size);

	char* getBitsetArray();
	int getSize();

};

#endif
