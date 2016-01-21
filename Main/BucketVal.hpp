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
	BucketVal(std::string ikey, unsigned range, unsigned ilocalDepth);     // Val HashTable
	BucketVal(Val_bdata* valdata);
	~BucketVal();

	void addBucketData(std::string key, unsigned range);
	void addBucketData(Val_bdata* valdata);

	void insert(std::string key, unsigned range);     // Validation HasthTable insert
	void insert(Val_bdata* valdata);

	Val_bdata* keySearch(std::string key);

};
