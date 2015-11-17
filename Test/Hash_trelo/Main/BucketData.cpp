#include "BucketData.hpp"

using namespace std;

BucketData::BucketData(unsigned t, unsigned offset)
{
	cout << "New BucketData created" << endl;
	tid = t;
	rangeArray.push_back(offset);
}

DArray<unsigned> BucketData::getrangeArray(){
	return rangeArray;
}

unsigned BucketData::getTid(){
	return tid;
}
