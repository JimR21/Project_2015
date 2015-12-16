#include "Bucket.hpp"
#include "Gen_HashTable.hpp"

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

Bucket::Bucket(unsigned ikey, int offset, unsigned ilocalDepth) : key(ikey), localDepth(ilocalDepth), tid_offset(offset)
{
    first = NULL;
    last = NULL;
    empty = false;
}

Bucket::Bucket(std::string ikey, unsigned ilocalDepth)
{
	vkey = ikey;
	localDepth = ilocalDepth;
	empty = false;
	data.insert();	// counter ++ pros to paron
}

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

// c0 hash insert
void Bucket::insert(unsigned ikey, unsigned tid, unsigned offset)
{
    key = ikey;
    empty = false;
    addBucketData(tid, offset);
}

// tid hash insert
void Bucket::insert(unsigned ikey, unsigned offset)
{
    key = ikey;
    empty = false;
    tid_offset = offset;
}

// val hash insert
void Bucket::insert(string ikey)
{
	vkey = ikey;
	empty = false;
	data.insert();	// auksanei ton counter pros to paron
	// cout << "Bika sto insert tou bucket" << endl;
}

BucketData* Bucket::getdataLast()   // NEW
{
    return last;
}