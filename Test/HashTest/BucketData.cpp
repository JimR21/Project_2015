#include "BucketData.hpp"

using namespace std;

BucketData::BucketData(unsigned t, unsigned offset)
{
	tid = t;
	rangeArray.push_back(offset);
}

DArray<unsigned> BucketData::getrangeArray(){
	return rangeArray;
}

unsigned BucketData::getTid(){
	return tid;
}

unsigned BucketData::getrangeArrayLast()
{
    return rangeArray.get(rangeArray.size()-1);
};
