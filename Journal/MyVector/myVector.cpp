#include <iostream>
#include <stdint.h>

using namespace std;

class DArray {
    uint64_t* arr;  // pointer to the first element of this myvec
    int capacity; // number of elements arr can hold (i.e. size of underlying array)
    int n;        // size of this myvec

    // Increases the capacity of the underlying array to be sz. If sz
    // is smaller than the current capacity then nothing is done.
    void increase_capacity(int sz) {
	    if (sz <= capacity) return;

	    uint64_t* new_arr = new uint64_t[sz];   // allocate a new array on the free store

	    for(int i = 0; i < capacity; ++i) { // copy old vector into new one
	      new_arr[i] = arr[i];
	    }
	    capacity = sz;                      // set the new capacity

	    delete[] arr;                       // delete the old vector
	    arr = new_arr;
    }

public:

    // create an empty vector
    DArray() : capacity(10), n(0) {
    	arr = new uint64_t[capacity];
    }

    int size() const {
        return n;
    }

    void push_back(uint64_t x) {
        if (n >= capacity) increase_capacity(2 * capacity);
        arr[n] = x;
        ++n;
    }

    void set(int i, uint64_t val) {
        //if (i < 0 || i >= n) error("range error");
        arr[i] = val;
    }

    // Pre: 0 <= i <= n
    // Post: x is at location i, and all previous elements from i
    //       onwards have been shifted to the right one position; the size of
    //       the vector is 1 larger than before calling insert


    uint64_t get(int i) const {
        //if (i < 0 || i >= n) error("range error");
        return arr[i];
    }

    ~DArray() {       // destructor
        delete[] arr;
    }

}; // class DArray
