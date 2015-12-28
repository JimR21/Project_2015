#ifndef VALIDATIONINDEX_HPP
#define VALIDATIONINDEX_HPP

#include "Val_listbucket.hpp"

class ValidationIndex
{
protected:
	Val_listbucket* start_bucket;
    Val_listbucket* last_bucket;
	unsigned size;


public:
	ValidationIndex();
	void insertValidation(ValidationQueries* val);
	void popValidation();	// deletes first node
	ValidationQueries* getHeadValidation();	// returns first node
	unsigned getSize();
};


#endif
