#ifndef VALIDATIONINDEX_HPP
#define VALIDATIONINDEX_HPP

#include "Val_listbucket.hpp"

class ValidationIndex
{
protected:
	Val_listbucket* start_bucket;
    Val_listbucket* last_bucket;
		Val_listbucket* iteratedBucket;
	unsigned size;


public:
	ValidationIndex();
	~ValidationIndex();
	void insertValidation(ValClass* val);
	void popValidation();	// deletes first node
	void fakePopValidation();
	ValClass* getHeadValidation();	// returns first node
	Val_listbucket* getNextValidation();
	Val_listbucket* getHeadBucket();
	unsigned getSize();
};


#endif
