#ifndef VAL_LISTBUCKET_HPP
#define VAL_LISTBUCKET_HPP

#include "mainStructs.hpp"

class ValidationIndex;

class Val_listbucket
{
    friend class ValidationIndex;
protected:
    ValidationQueries val;
    Val_listbucket* next;


public:
};


#endif
