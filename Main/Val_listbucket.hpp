#ifndef VAL_LISTBUCKET_HPP
#define VAL_LISTBUCKET_HPP

#include "valClass.hpp"
#include <iostream>

class ValidationIndex;

class Val_listbucket
{
    friend class ValidationIndex;
protected:
    ValClass* val;
    Val_listbucket* next;

public:
	// inline constructor gia na min ftiaxnw .cpp mono gia ton constructor
	inline Val_listbucket(ValClass* validation);
};

#endif

Val_listbucket::Val_listbucket(ValClass* validation){
	val = validation;
	next = NULL;
}
