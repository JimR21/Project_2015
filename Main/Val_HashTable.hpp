#ifndef VAL_HASHTABLE_HPP
#define VAL_HASHTABLE_HPP
#include "Gen_HashTable.hpp"
#include <string>

class Val_HashTable : public Gen_HashTable
{
public:
    void insert(std::string key, int data);

    int getbdata(std::string key);
};


#endif
