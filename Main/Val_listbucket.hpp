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
	Val_listbucket(ValClass* validation);
	~Val_listbucket();
};

#endif
