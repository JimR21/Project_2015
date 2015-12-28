#ifndef VAL_LISTBUCKET_HPP
#define VAL_LISTBUCKET_HPP

#include "mainStructs.hpp"
#include <iostream>

class ValidationIndex;

class Val_listbucket
{
    friend class ValidationIndex;
protected:
    ValidationQueries* val;
    Val_listbucket* next;

public:
	// inline constructor gia na min ftiaxnw .cpp mono gia ton constructor
	inline Val_listbucket(ValidationQueries* validation);
};

#endif

Val_listbucket::Val_listbucket(ValidationQueries* validation){
	val = validation;
	next = NULL;
}
