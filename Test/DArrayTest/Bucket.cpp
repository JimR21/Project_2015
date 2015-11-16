#include "Bucket.hpp"
#include "HashTable.hpp"

using namespace std;

Bucket::Bucket()
{
    localDepth = 7;     // Equal to globalDepth at start
    key = 0;
    data = 0;
    empty = true;
}

Bucket::Bucket(unsigned ikey, int idata, unsigned ilocalDepth) : key(ikey), data(idata), localDepth(ilocalDepth)
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
