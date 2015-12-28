#include <string>
#include "Val_bdata.hpp"

class BucketVal
{
public:
	unsigned int localDepth;
    bool empty;

	Val_bdata* first;
	Val_bdata* last;
	int counter;	// number of Val_bdata

	BucketVal();
	BucketVal(unsigned ilocalDepth);
	BucketVal(std::string ikey, unsigned ilocalDepth);     // Val HashTable
	BucketVal(Val_bdata* valdata);

	void addBucketData(std::string key);
	void addBucketData(Val_bdata* valdata);

	void insert(std::string key);     // Validation HasthTable insert
	void insert(Val_bdata* valdata);

	Val_bdata* keySearch(std::string key);

};
