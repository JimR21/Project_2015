#include <stdio.h>
#include <limits.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "bitset.hpp"

////////////////////////////////////////////////////////////////
void bf_array_update(int index, char *bloom_array)	// updates a specifix index bit of the bf array
{
	int bit_index = index / CHAR_BIT;		// which char
	int bit_number = index % CHAR_BIT;		// which bit of this char

	bloom_array[bit_index] = bloom_array[bit_index] | (1 << (7 - bit_number));	// update !!
}
////////////////////////////////////////////////////////////////
// APO INTERNET GIA DEBUG //
void printbincharpad(char c)	// Prints the bits of a character
{
	int i;

    for (i = 7; i >= 0; --i)
    {
        putchar( (c & (1 << i)) ? '1' : '0' );
    }
    putchar('\n');
}
////////////////////////////////////////////////////////////////
int bf_array_check(int index, char *bloom_array)	// checks if index bit is 1 or 0
{
	int bit_index = index / CHAR_BIT;		// which char
	int bit_number = index % CHAR_BIT;		// which bit of this char

	if (bloom_array[bit_index] & (1 << bit_number))
		return 1;
	else
		return 0;
}
