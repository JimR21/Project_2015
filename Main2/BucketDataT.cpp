/* BucketData gia hash me kleidi to tid kai value to offset */

#include "BucketDataT.hpp"

using namespace std;

BucketDataT::BucketDataT(unsigned ioffset)
{
	offset = ioffset;
}

unsigned BucketDataT::getOffset(){
	return offset;
}
