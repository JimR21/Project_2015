#ifndef DARRAY_HPP
#define DARRAY_HPP

#include <iostream>
#include <stdint.h>
#include <cstring>

template <class T>
class DArray {
    T* arr;  // pointer to the first element of this myvec
    int capacity; // number of elements arr can hold (i.e. size of underlying array)
    int n;        // size of this myvec

    void increase_capacity(int sz);

public:

	DArray();
    DArray(unsigned capacity);
    ~DArray();
    int size() ;
    void changeSize(int i);
    void push_back(T const& x);
    void set(int i, T val);
    T get(int i) const;
    T getLast() const;
    T popGetLast();
    void popAt(int i);  // Removes data at position i
    void fastPopAt(unsigned i); // Fast remove
    void popLast();     // Removes last
    void specialDouble();
    T& operator[](int i);
    const T& operator[](int i) const;


}; // class DArray

//=========================================================

// create an empty vector
template <class T>
// DArray<T>::DArray() : capacity(32768), n(0) {
DArray<T>::DArray() : capacity(32768), n(0) {
	arr = new T[capacity];
}
//=========================================================
// create an empty vector
template <class T>
// DArray<T>::DArray() : capacity(32768), n(0) {
DArray<T>::DArray(unsigned icapacity) : capacity(icapacity), n(0) {
	arr = new T[capacity];
}
//=========================================================

template <class T>
DArray<T>::~DArray() {       // destructor
    delete[] arr;
}


//=========================================================

template <class T>
int DArray<T>::size(){
    return n;
}

//=========================================================

template <class T>
void DArray<T>::increase_capacity(int sz) {     // x2
    if (sz <= capacity)
               return;
     T* new_arr = new T[sz];     // allocate a new array on the free store
         memcpy(new_arr, arr, n * sizeof(T) );

     capacity = sz;                      // set the new capacity
     delete[] arr;                       // delete the old vector
     arr = new_arr;
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


template <class T>
T DArray<T>::getLast() const {
    //if (i < 0 || i >= n) error("range error");
    return arr[n-1];
}

template <class T>
T DArray<T>::popGetLast() {
    if(n == 0)
        return NULL;
    T res=arr[n-1];
    arr[n-1] = 0;
    n--;
    return res;
}


//=========================================================

template <class T>
void DArray<T>::popAt(int i) {
    if(i >= n)
        return;                  // i bigger than size
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
    return;
}

//=========================================================

template <class T>
void DArray<T>::fastPopAt(unsigned i) {
    if(i >= n)
        return;                  // i bigger than size
    if(i == n-1)
    {
        arr[i] = 0;
    }
    else
    {
        arr[i] = arr[n-1];
        arr[n-1] = 0;
    }
    n--;
    return;
}

//=========================================================

template <class T>
void DArray<T>::popLast() {
    if(n == 0)
        return;
    arr[n-1] = 0;
    n--;
    return;
}





//=========================================================

template <class T>
void DArray<T>::specialDouble()
{
    int new_size = n*2;
    if(new_size > capacity)
        increase_capacity(2 * capacity);

    std::memcpy(&arr[n], &arr[0], sizeof(T)*n);
    n = new_size;
}

#endif
