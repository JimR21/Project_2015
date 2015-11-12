#include <iostream>
#include <stdint.h>
#include "DArray.h"

using namespace std;

void DArray::increase_capacity(int sz) {
    	if (sz <= capacity)
	return;

    uint64_t* new_arr = new uint64_t[sz];   // allocate a new array on the free store

    for(int i = 0; i < capacity; ++i) { // copy old vector into new one
      new_arr[i] = arr[i];
    }
    capacity = sz;                      // set the new capacity

    delete[] arr;                       // delete the old vector
    arr = new_arr;
}


// create an empty vector
DArray::DArray() : capacity(10), n(0) {
	arr = new uint64_t[capacity];
}

int DArray::size() {
    return n;
}

void DArray::push_back(uint64_t x) {
    if (n >= capacity) increase_capacity (2 * capacity);
    arr[n] = x;
    ++n;
}

void DArray::set(int i, uint64_t val) {
    //if (i < 0 || i >= n) error("range error");
    arr[i] = val;
}


uint64_t DArray::get(int i) {
    //if (i < 0 || i >= n) error("range error");
    return arr[i];
}

DArray::~DArray() {       // destructor
    delete[] arr;
}
