#include <string>
#include "Val_bdata.hpp"

class BucketVal
{
public:

	unsigned int localDepth;
    bool empty;

	// Val hash table key and data
	std::string vkey;
	Val_bdata* data;

	BucketVal();
	BucketVal(unsigned ilocalDepth);
	BucketVal(std::string ikey, unsigned ilocalDepth);     // Val HashTable

	void insert(std::string key);     // Validation HasthTable insert

};
