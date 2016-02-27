#ifndef VALIDATIONINDEX_HPP
#define VALIDATIONINDEX_HPP

#include "ValidationNode.hpp"

class ValidationIndex
{
protected:
	DArray<ValidationNode*> validationList('a', 'a');
	DArray<bool> resultValidationList;

public:
	ValidationIndex();
	~ValidationIndex();
	void addValidation(ValClass* val);
	void popValidation();	// deletes first node
	void fakePopValidation();
	ValClass* getHeadValidation();	// returns first node
	Val_listbucket* getNextValidation();
	Val_listbucket* getHeadBucket();
	unsigned getSize();
};


#endif
