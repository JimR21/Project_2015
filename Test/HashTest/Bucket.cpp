#include "Bucket.hpp"
#include "HashTable.hpp"

using namespace std;

unsigned BucketData::getTid()
{
    return tid;
};

DArray<unsigned> BucketData::getrangeArray()
{
    return rangeArray;
};

Bucket::Bucket()
{
    localDepth = 7;     // Equal to globalDepth at start
    key = 0;
    //data = 0;
    empty = true;
}

Bucket::Bucket(unsigned ikey, unsigned ilocalDepth) : key(ikey), localDepth(ilocalDepth)
{
    empty = false;
}

Bucket::~Bucket()
{

}

void Bucket::insert(unsigned ikey, int idata)
{
    key = ikey;
    data = idata;
    empty = false;
}
