#ifndef VAL_HASHTABLE_HPP
#define VAL_HASHTABLE_HPP
#include "Gen_HashTable.hpp"
#include <string>

class Val_HashTable : public Gen_HashTable
{
private:

public:
	Val_HashTable();
	int hashFunction(const std::string& key);
    void insert(std::string key);
    int getbdata(std::string key);

};


#endif
