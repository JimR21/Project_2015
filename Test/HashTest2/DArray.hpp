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
    int size();
    void push_back(T const& x);
    void set(int i, T val);
    T get(int i) const;
    ~DArray();
    T& operator[](int i);
    const T& operator[](int i) const;


}; // class DArray

template <class T>
void DArray<T>::increase_capacity(int sz) {
    	if (sz <= capacity)
	return;

    T* new_arr = new T[sz];   // allocate a new array on the free store

    for(int i = 0; i < capacity; ++i) { // copy old vector into new one
      new_arr[i] = arr[i];
    }
    capacity = sz;                      // set the new capacity

    delete[] arr;                       // delete the old vector
    arr = new_arr;
}


// create an empty vector
template <class T>
DArray<T>::DArray() : capacity(10), n(0) {
	arr = new T[capacity];
}

template <class T>
int DArray<T>::size() {
    return n;
}

template <class T>
void DArray<T>::push_back(T const& x) {
    if (n >= capacity) increase_capacity (2 * capacity);
    arr[n] = x;
    ++n;
}

template <class T>
void DArray<T>::set(int i, T val) {
    //if (i < 0 || i >= n) error("range error");
    arr[i] = val;
}

template <class T>
T DArray<T>::get(int i) const {
    //if (i < 0 || i >= n) error("range error");
    return arr[i];
}

template <class T>
DArray<T>::~DArray() {       // destructor
    delete[] arr;
}

template <class T>
T& DArray<T>::operator[](int i) {
    return arr[i];
}

template <class T>
  const T& DArray<T>::operator[](int i) const {
    return arr[i];
}
