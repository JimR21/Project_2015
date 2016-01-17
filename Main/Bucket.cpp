#include "Bucket.hpp"
#include "Gen_HashTable.hpp"

using namespace std;

Bucket::Bucket(): key(0), localDepth(7), empty(true), first(NULL), last(NULL)
{}

Bucket::Bucket(unsigned ilocalDepth): key(0), localDepth(ilocalDepth), empty(true), first(NULL), last(NULL)
{}

Bucket::Bucket(unsigned ikey, unsigned tid, unsigned offset, unsigned ilocalDepth) : key(ikey), localDepth(ilocalDepth), empty(false), first(NULL), last(NULL){
    addBucketData(tid, offset);
}

Bucket::Bucket(unsigned ikey, int offset, unsigned ilocalDepth) : key(ikey), localDepth(ilocalDepth), empty(false), first(NULL), last(NULL), tid_offset(offset)
{}

Bucket::~Bucket()
{
    BucketData* temp = first;
    BucketData* tempnext;
    do
    {
        if(temp != NULL)
        {
            tempnext = temp->next;
            delete temp;
            temp = tempnext;
        }
        if(temp == NULL)
            break;
    }while(1);
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
                temp->second_of = true;
                flag = false;
                break;
            }
            if(temp->next == NULL)
                break;
            temp = temp->next;
        }while(1);
        if(flag)
        {
            temp->next = new BucketData(tid, offset);
            last = temp->next;
        }
    }
}

void Bucket::insert(unsigned ikey, unsigned tid, unsigned offset)
{
    key = ikey;
    empty = false;
    addBucketData(tid, offset);
}

void Bucket::insert(unsigned ikey, unsigned offset)
{
    key = ikey;
    empty = false;
    tid_offset = offset;
}

BucketData* Bucket::getdataLast()   // NEW
{
    return last;
}
