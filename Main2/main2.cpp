#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>
#include <string>
#include <chrono>
#include "Journal.hpp"
#include "Bucket.hpp"
#include "HashTable.hpp"
#include <fstream>

using namespace std;
//=====================================================
//================== MAIN PROGRAM =====================
//=====================================================
int main(int argc, char **argv) {

	srand(time(NULL));
	HashTable *table = new HashTable();

	for (uint64_t i = 0; i < 400; i++){
		unsigned off = rand() % 1000;
		cout << "i: " << i << ", offset: " << off << endl;
		table->tInsert(i, off);
	}

	for (uint64_t i = 0; i < 400; i++){
		cout << "i: " << i << ", off: " << table->getTidVal(i) << endl;
	}
    return 0;
}
