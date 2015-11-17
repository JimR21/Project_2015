#include "Bucket.hpp"
#include "HashTable.hpp"

using namespace std;

Bucket::Bucket()
{
    localDepth = 7;     // Equal to globalDepth at start
    key = 0;
    //data = 0;
    empty = true;
}

Bucket::Bucket(unsigned ikey, BucketData* bdata, unsigned ilocalDepth) : key(ikey), localDepth(ilocalDepth)
{
    key = ikey;
	data.push_back(bdata);
    empty = false;
}

Bucket::~Bucket()
{

}

void Bucket::insert(unsigned ikey, BucketData* idata)
{
    key = ikey;
	data.push_back(idata);
    // data = idata;
    empty = false;
}
