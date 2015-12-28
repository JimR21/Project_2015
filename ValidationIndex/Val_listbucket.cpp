#include "Val_listbucket.hpp"
#include <iostream>

Val_listbucket::Val_listbucket(ValidationQueries validation){
	val = validation;
	next = NULL;
}
