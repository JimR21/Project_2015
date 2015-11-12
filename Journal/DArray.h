#include <iostream>
#include <stdint.h>

using namespace std;

class DArray {
    uint64_t* arr;  // pointer to the first element of this myvec
    int capacity; // number of elements arr can hold (i.e. size of underlying array)
    int n;        // size of this myvec

    void increase_capacity(int sz);

public:

    DArray();
    int size();
    void push_back(uint64_t x);
    void set(int i, uint64_t val);
    uint64_t get(int i);
    ~DArray();

}; // class DArray
