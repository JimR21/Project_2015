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

	first = new Val_bdata(ikey);
	first->insert();	// counter ++ pros to paron
	last = first;
	counter = 1;
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
	//valdata->next = NULL;
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
