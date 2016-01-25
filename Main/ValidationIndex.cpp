#include "ValidationIndex.hpp"
#include <iostream>

using namespace std;

ValidationIndex::ValidationIndex(){
	start_bucket = NULL;
	last_bucket = NULL;
	iteratedBucket=NULL;
	size = 0;
}

ValidationIndex::~ValidationIndex()
{
	if(size == 0)
		return;
	Val_listbucket* temp = start_bucket;
	Val_listbucket* prev = start_bucket;
	do
	{
		temp = temp->next;
		delete prev;
		prev = temp;
	}while(temp != NULL);
}

void ValidationIndex::insertValidation(ValClass* val){

	Val_listbucket* newBucket = new Val_listbucket(val);

	if (!size){
		start_bucket = newBucket;
		iteratedBucket=newBucket;
	}
	else {
		last_bucket->next = newBucket;
	}
	last_bucket = newBucket;
	size++;
}

void ValidationIndex::popValidation(){

	if (size != 0){
		Val_listbucket* temp = start_bucket;
		start_bucket = start_bucket->next;
		size--;
		delete temp;
	}
	// else
	// 	throw runtime_error("Trying to pop from empty list");
}


void ValidationIndex::fakePopValidation(){       //like popValidation but only move the pointer to get the next pointer with getNextValidation()
	if (size != 0){
		Val_listbucket* temp = iteratedBucket;
		iteratedBucket = iteratedBucket->next;

	}

}

ValClass* ValidationIndex::getHeadValidation(){
	// if (size == 0)
	// 	// throw runtime_error("Trying to get head from empty list");
	return start_bucket->val;
}

Val_listbucket* ValidationIndex::getHeadBucket(){                
	// if (size == 0)
	// 	// throw runtime_error("Trying to get head from empty list");
	return start_bucket;
}


Val_listbucket* ValidationIndex::getNextValidation(){
	return iteratedBucket;
}

unsigned ValidationIndex::getSize(){
	return size;
}
