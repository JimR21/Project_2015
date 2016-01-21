#include "BucketVal.hpp"
#include <string>

using namespace std;

BucketVal::BucketVal(){
	localDepth = 7;     // Equal to globalDepth at start
    empty = true;
	first = NULL;
	last = NULL;
	counter = 0;
}

BucketVal::BucketVal(unsigned ilocalDepth) : localDepth(ilocalDepth)
{
    empty = true;
	first = NULL;
	last = NULL;
	counter = 0;
}

BucketVal::BucketVal(std::string ikey, unsigned range, unsigned ilocalDepth)
{
	localDepth = ilocalDepth;
	empty = false;
	first = new Val_bdata(ikey, range);
	last = first;
	counter = 1;
}

BucketVal::BucketVal(Val_bdata* valdata)
{

}

BucketVal::~BucketVal()
{
	if(first != NULL)
	{
		Val_bdata* temp = first;
		Val_bdata* prev = first;
		do
		{
			prev = temp;
			temp = temp->next;
			delete prev;
		} while(temp != NULL);
	}
}

void BucketVal::addBucketData(string key, unsigned range)
{
    if(first == NULL)
    {
        first = new Val_bdata(key, range);
        last = first;
    }
    else
    {
        Val_bdata* temp = last;
        temp->next = new Val_bdata(key, range);
        last = temp->next;
    }
}

void BucketVal::addBucketData(Val_bdata* valdata)
{
	//valdata->next = NULL;
	if(first == NULL)
	{
		first = valdata;
		last = valdata;
	}
	else
	{
		Val_bdata* temp = last;
		temp->next = valdata;
		last = temp->next;
	}
}

// val hash insert
void BucketVal::insert(string ikey, unsigned range)
{
	empty = false;
	addBucketData(ikey, range);
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
	if(key.compare(first->key) == 0)
		return first;
	do {
		if(key.compare(temp->key) == 0)
			return temp;
		if(temp->next == NULL)
			return NULL;
		temp = temp->next;
	} while(1);
}
