#include "ValidationIndex.hpp"
#include <iostream>

using namespace std;

ValidationIndex::ValidationIndex(){
	start_bucket = NULL;
	last_bucket = NULL;
	size = 0;
}

void ValidationIndex::insertValidation(ValidationQueries* val){

	Val_listbucket* newBucket = new Val_listbucket(val);

	if (!size)
		start_bucket = last_bucket = newBucket;
	else {
		last_bucket->next = newBucket;
		last_bucket = newBucket;
	}
	size++;
}

void ValidationIndex::popValidation(){

	if (size != 0){
		Val_listbucket* temp = start_bucket;
		start_bucket = start_bucket->next;
		size--;
		delete temp;
	}
	else
		throw runtime_error("Trying to pop from empty list");
}

ValidationQueries* ValidationIndex::getHeadValidation(){
	if (size == 0)
		throw runtime_error("Trying to get head from empty list");
	return start_bucket->val;
}

unsigned ValidationIndex::getSize(){
	return size;
}
