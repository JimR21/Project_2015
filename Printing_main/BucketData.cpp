#include "BucketData.hpp"

using namespace std;

BucketData::BucketData(unsigned t, unsigned offset)
{
	tid = t;
	offsets[0] = offset;
	second_of = false;
	next = NULL;
}

unsigned * BucketData::getOffsets(){
	return offsets;
}

unsigned BucketData::getTid(){
	return tid;
}

unsigned BucketData::getOffsetLast()
{
    if(second_of == true)
		return offsets[1];
	return offsets[0];
};
