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

	// TODO: setters getters xwris arrays

	void setBitsetValue(int index, char *array);	// set Bitset index to 1

	unsigned getBitsetValue(unsigned index, char *array);		// get the value of this index

	void setBitsetValue(unsigned index);	// set Bitset index to 1

	unsigned getBitsetValue(unsigned index);		// get the value of this index

	void printBitset(char c); // print the array

	void validateBitset(char *array, int size);

	bool validateBitset();

	void addBitset(Bitset * bit);

	char* getBitsetArray();

	int getSize();

};

#endif
