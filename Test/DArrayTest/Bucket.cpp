#include "Bucket.hpp"
#include "HashTable.hpp"

using namespace std;

Bucket::Bucket()
{
    key = 0;
    data = 0;
    empty = true;
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

unsigned Bucket::getkey()
{
    return key;
}
