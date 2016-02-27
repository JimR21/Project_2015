#ifndef DARRAY_HPP
#define DARRAY_HPP

#include <cstring>
#include <iostream>
#include <stdint.h>
#include <pthread.h>
#include "options.hpp"


template <class T>
class DArray {
    T* arr;  // pointer to the first element of this myvec
    int capacity; // number of elements arr can hold (i.e. size of underlying array)
    int n;        // size of this myvec

	//======================================
	// mutex kai cond_var otan exw threads
	//======================================
	#if VAL_THREADS == 1
		pthread_mutex_t arr_mutex;
		pthread_cond_t  arr_condv;
		unsigned index; 	// index gia to fake pop
	#endif

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

	//======================================
	// thread safe DArray functions
	//======================================
	#if VAL_THREADS == 1
		DArray(char c, char threadsafe); // 2 orismata giati ama evazes mono 1 p.x char threadsafe
								 			 // otan tin kalouses me int elege ambiguous call asxeta pou kaleis
											 // me int. (ligo bakaliko alla doulevei)
		void destroy();
		void wakeUpWorkers();
		void safe_push_back(T const& x);
		T safe_popGetLast();
		T safe_fake_popGetLast();
		void resetIndex();

        bool mainFlag;
	#endif

}; // class DArray

//=========================================================
// create an empty vector
template <class T>
DArray<T>::DArray() : capacity(32768), n(0) {
	arr = new T[capacity];
}
//=========================================================
// create an empty vector
template <class T>
DArray<T>::DArray(unsigned icapacity) : capacity(icapacity), n(0) {
	arr = new T[capacity];
}
//=========================================================

#if VAL_THREADS == 1
	//=========================================================
	template <class T>
	DArray<T>::DArray(char c, char threadsafe) : capacity(5000), n(0), index(0) {
		arr = new T[capacity];

		pthread_mutex_init(&arr_mutex, NULL);	// initialize mutex
		pthread_cond_init(&arr_condv, NULL);	// initialize cond var

        mainFlag = false;
	}
	//=========================================================
	template <class T>				  // prin to delete tha ginetai kai auto
	void DArray<T>::destroy() {       // destroy mutex and cond var
		pthread_mutex_destroy(&arr_mutex);
    	pthread_cond_destroy(&arr_condv);
	}
	//=========================================================
	template <class T>
	void DArray<T>::wakeUpWorkers() {
		pthread_cond_broadcast(&arr_condv);	// wake up workers
	}
	//=========================================================
	template <class T>
	void DArray<T>::safe_push_back(T const& x) {
		pthread_mutex_lock(&arr_mutex);	// lock arr
		    if (n >= capacity) increase_capacity (2 * capacity);
		    arr[n] = x;
		    n++;
			index = n;
		pthread_cond_signal(&arr_condv);	// wake 1 worker
	    pthread_mutex_unlock(&arr_mutex);	// unlock arr
	}
	//=========================================================
	template <class T>
	T DArray<T>::safe_popGetLast() {
		pthread_mutex_lock(&arr_mutex);	// lock arr
			while (n == 0) {	// no items available to pop
				pthread_cond_wait(&arr_condv, &arr_mutex);	// release mutex & wait till the condition is signaled
			}
			T item = arr[n-1];
			arr[n-1] = 0;
		    n--;
			index = n;
		pthread_mutex_unlock(&arr_mutex);
	    return item;
	}
	//=========================================================
	//=========================================================
	template <class T>
	T DArray<T>::safe_fake_popGetLast() {
		pthread_mutex_lock(&arr_mutex);	// lock arr
			while (mainFlag == false || index == 0) {	// no items available to pop
				pthread_cond_wait(&arr_condv, &arr_mutex);	// release mutex & wait till the condition is signaled
			}
			T item = arr[index-1];
		    index--;
		pthread_mutex_unlock(&arr_mutex);
	    return item;
	}
	//=========================================================
	template <class T>
	void DArray<T>::resetIndex(){
		index = n;
	}
	//=========================================================

#endif
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
	#if VAL_THREADS == 1
		index = n;
	#endif
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
