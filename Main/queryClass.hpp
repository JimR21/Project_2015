#ifndef QUERYCLASS_HPP
#define QUERYCLASS_HPP
#include <inttypes.h>

#include "columnClass.hpp"
#include "Journal.hpp"

extern Journal** Journals;
extern unsigned forget;

std::string stringBuilder(int start, int end, int col, int op, uint64_t value);

class QueryClass
{
public:
    // The relation
    uint32_t relationId;
    // The number of bound columns
    uint32_t columnCount;
    // The bindings
    ColumnPtr * columns;

    ~QueryClass();

    QueryClass(const uint32_t &relId, const uint32_t &colCount, ColumnPtr * cols);

    bool validate(uint64_t &form, uint64_t &to);

#if VAL_HASHTABLE == 1
    bool hashValidate(uint64_t &from, uint64_t &to);
#endif
};

typedef QueryClass* QueryPtr;

#endif
