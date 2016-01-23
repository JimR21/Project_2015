#ifndef QUERYCLASS_HPP
#define QUERYCLASS_HPP
#include <inttypes.h>
#include "columnClass.hpp"
#include "Journal.hpp"


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

    bool validate(Journal &journal, uint64_t &form, uint64_t &to);
};

typedef QueryClass* QueryPtr;

#endif
