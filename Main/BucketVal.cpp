#include "BucketVal.hpp"
#include <string>

using namespace std;

BucketVal::BucketVal(){
	localDepth = 7;     // Equal to globalDepth at start
    empty = true;
	data = NULL;
	// data = new Val_bdata();
}

BucketVal::BucketVal(unsigned ilocalDepth) : localDepth(ilocalDepth)
{
    empty = true;
	data = NULL;
	// data = new Val_bdata();
}

BucketVal::BucketVal(std::string ikey, unsigned ilocalDepth)
{
	vkey = ikey;
	localDepth = ilocalDepth;
	empty = false;
	data = NULL;
	if (data == NULL){
		data = new Val_bdata();
		data->insert();	// counter ++ pros to paron
	}
}

// // val hash insert
void BucketVal::insert(string ikey)
{
	vkey = ikey;
	empty = false;
	if (data == NULL){
		data = new Val_bdata();
		data->insert();	// auksanei ton counter pros to paron
	}
}
