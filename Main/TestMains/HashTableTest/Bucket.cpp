#include "Bucket.hpp"
#include "HashTable.hpp"

using namespace std;

Bucket::Bucket()
{
    localDepth = 7;     // Equal to globalDepth at start
    key = 0;
    empty = true;
}

Bucket::Bucket(unsigned ilocalDepth) : localDepth(ilocalDepth)
{
    key = 0;
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
    for(int i = 0; i < data.size(); i++)
    {
        if(data.get(i) != NULL)
            delete data.get(i);
    }
}

void Bucket::insert(unsigned ikey, BucketData* idata)
{
    key = ikey;
	data.push_back(idata);
    // data = idata;
    empty = false;
}

BucketData* Bucket::getdataLast()   // NEW
{
    return data.get(data.size()-1);
}
