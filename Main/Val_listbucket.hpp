#ifndef VAL_LISTBUCKET_HPP
#define VAL_LISTBUCKET_HPP

#include "valClass.hpp"
#include <iostream>

class Val_listbucket
{
protected:
    ValClass* val;
    bool result;              //threads compute the value and saved it
    Val_listbucket* next;

public:
	Val_listbucket(ValClass* validation);
	~Val_listbucket();
	ValClass* getVal();
	bool getResult();
	void setResult(bool conflict);
};

#endif
