#include "BucketVal.hpp"
#include <string>

using namespace std;

BucketVal::BucketVal(){
	localDepth = 7;     // Equal to globalDepth at start
    empty = true;
	first = NULL;
	last = NULL;
}

BucketVal::BucketVal(unsigned ilocalDepth) : localDepth(ilocalDepth)
{
    empty = true;
	first = NULL;
	last = NULL;
}

BucketVal::BucketVal(std::string ikey, unsigned ilocalDepth)
{
	localDepth = ilocalDepth;
	empty = false;

	first = new Val_bdata();
	first->insert();	// counter ++ pros to paron
	last = first;
}

BucketVal::BucketVal(Val_bdata* valdata)
{

}

void BucketVal::addBucketData(string key)
{
    if(first == NULL)
    {
        first = new Val_bdata(key);
        last = first;
    }
    else
    {
        Val_bdata* temp = last;
        temp->next = new Val_bdata(key);
        last = temp->next;
    }
}

void BucketVal::addBucketData(Val_bdata* valdata)
{
	if(first == NULL)
	{
		first = valdata;
		last = first;
	}
	else
	{
		Val_bdata* temp = last;
		temp->next = valdata;
		last = temp->next;
	}
}

// val hash insert
void BucketVal::insert(string ikey)
{
	empty = false;
	addBucketData(ikey);
	counter++;
}

void BucketVal::insert(Val_bdata* valdata)
{
	empty = false;
	addBucketData(valdata);
	counter++;
}

Val_bdata* BucketVal::keySearch(string key)
{
	if(first == NULL)
		return NULL;
	Val_bdata* temp = first;
	do {
		if(key.compare(first->key) == 0)
			return first;
		if(temp->next == NULL)
			return NULL;
	} while(1);
}
