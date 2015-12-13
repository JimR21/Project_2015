#ifndef TID_HASHTABLE_HPP
#define TID_HASHTABLE_HPP
#include "Gen_HashTable.hpp"

class Tid_HashTable : public Gen_HashTable
{
public:
    void insert(unsigned key, unsigned int offset);

    unsigned getoffset(unsigned key);
};


#endif
