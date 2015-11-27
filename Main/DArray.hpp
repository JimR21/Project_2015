#ifndef DARRAY_HPP
#define  DARRAY_HPP

#include <iostream>
#include <stdint.h>

template <class T>
class DArray {
    T* arr;  // pointer to the first element of this myvec
    int capacity; // number of elements arr can hold (i.e. size of underlying array)
    int n;        // size of this myvec

    void increase_capacity(int sz);

public:

    DArray();
    int size() ;
    void changeSize(int i);
    void push_back(T const& x);
    void set(int i, T val);
    T get(int i) const;
    int popAt(int i);  // Removes data at position i. Returns 1 on success, -1 if i > n
    int popLast();     // Removes last. 1 on succes, -1 if empty
    ~DArray();
    T& operator[](int i);
    const T& operator[](int i) const;


}; // class DArray

//=========================================================

template <class T>
void DArray<T>::increase_capacity(int sz) {     // x2
	if (sz <= capacity)
	      return;
    T* new_arr;
    {
      new_arr = new T[sz];   // allocate a new array on the free store
    }
    for(int i = 0; i < capacity; ++i) { // copy old vector into new one
      new_arr[i] = arr[i];
    }
    capacity = sz;                      // set the new capacity

    delete[] arr;                       // delete the old vector
    arr = new_arr;
}

//=========================================================
// create an empty vector
template <class T>
DArray<T>::DArray() : capacity(32768), n(0) {
	arr = new T[capacity];
}

//=========================================================

template <class T>
int DArray<T>::size(){
    return n;
}

//=========================================================

template <class T>
void DArray<T>::changeSize(int i) {
    n = i;
}

//=========================================================

template <class T>
void DArray<T>::push_back(T const& x) {
    if (n >= capacity) increase_capacity (2 * capacity);
    arr[n] = x;
    n++;
}

//=========================================================

template <class T>
void DArray<T>::set(int i, T val) {
    //if (i < 0 || i >= n) error("range error");
    arr[i] = val;
}

//=========================================================

template <class T>
T DArray<T>::get(int i) const {
    //if (i < 0 || i >= n) error("range error");
    return arr[i];
}

//=========================================================

template <class T>
int DArray<T>::popAt(int i) {
    if(i >= n)
        return -1;                  // i bigger than size
    if(i == n-1)
    {
        arr[n-1] = 0;
    }
    else
    {
        int j;
        for(j = i; j < n-1; j++)
        {
            arr[j] = arr[j+1];
        }
        arr[j] = 0;
    }
    n--;
    return 1;
}

//=========================================================

template <class T>
int DArray<T>::popLast() {
    if(n == 0)
        return -1;
    arr[n-1] = 0;
    n--;
    return 1;
}

//=========================================================

template <class T>
DArray<T>::~DArray() {       // destructor
    delete[] arr;
}

//=========================================================

#endif
