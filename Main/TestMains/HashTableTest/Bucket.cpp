#include "Bucket.hpp"
#include "HashTable.hpp"

using namespace std;

Bucket::Bucket()
{
    localDepth = 7;     // Equal to globalDepth at start
    key = 0;
    empty = true;
    first = NULL;
    last = NULL;
}

Bucket::Bucket(unsigned ilocalDepth) : localDepth(ilocalDepth)
{
    key = 0;
    empty = true;
    first = NULL;
    last = NULL;
}

Bucket::Bucket(unsigned ikey, unsigned tid, unsigned offset, unsigned ilocalDepth) : key(ikey), localDepth(ilocalDepth)
{
    first = NULL;
    last = NULL;
    empty = false;
    addBucketData(tid, offset);
}

Bucket::~Bucket()
{

}

void Bucket::addBucketData(unsigned tid, unsigned offset)
{
    if(first == NULL)
    {
        first = new BucketData(tid, offset);
        last = first;
    }
    else
    {
        bool flag = true;   // An true tha prostethei neo Bucket data
        BucketData* temp = first;
        do
        {
            if(temp->tid == tid)
            {
                temp->offsets[1] = offset;
                flag = false;
                break;
            }
            temp = temp->next;
        }while(temp != NULL);
        if(flag)
        {
            temp = new BucketData(tid, offset);
            last = temp;
        }
    }
}

void Bucket::insert(unsigned ikey, unsigned tid, unsigned offset)
{
    key = ikey;
    empty = false;
    addBucketData(tid, offset);
}

BucketData* Bucket::getdataLast()   // NEW
{
    return last;
}
