#include "Val_listbucket.hpp"
#include <iostream>

Val_listbucket::Val_listbucket(ValClass* validation){
	val = validation;
	next = NULL;
	result=-1;
}

Val_listbucket::~Val_listbucket()
{
	delete val;
}

ValClass* Val_listbucket::getVal(){
	return val;
}
bool Val_listbucket::getResult(){
	return result;
}
void Val_listbucket::setResult(bool conflict){
	result=conflict;
}
