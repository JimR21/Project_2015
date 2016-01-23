#include "Val_listbucket.hpp"
#include <iostream>

Val_listbucket::Val_listbucket(ValClass* validation){
	val = validation;
	next = NULL;
}

Val_listbucket::~Val_listbucket()
{
	delete val;
}
