#include <iostream>
#include <malloc.h>
#include "bitset.hpp"

using namespace std;

int main(){

	char *bitset = (char*)malloc(1);	// allocate 1char = 8bits

	bf_array_update(2, bitset);	// updates a specifix index bit of the bf array
	printbincharpad(*bitset);
	bf_array_update(5, bitset);	// updates a specifix index bit of the bf array
	printbincharpad(*bitset);
	bf_array_update(3, bitset);	// updates a specifix index bit of the bf array
	printbincharpad(*bitset);
	bf_array_update(7, bitset);	// updates a specifix index bit of the bf array
	printbincharpad(*bitset);

	int result = bf_array_check(2, bitset);		// 1
	cout << "Result: " << result << endl;
	result = bf_array_check(1, bitset);			// 0
	cout << "Result: " << result << endl;


	return 0;
}
