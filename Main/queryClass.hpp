#ifndef QUERYCLASS_HPP
#define QUERYCLASS_HPP
#include <inttypes.h>
#include "columnClass.hpp"


class QueryClass
{
public:
    // The relation
    uint32_t relationId;
    // The number of bound columns
    uint32_t columnCount;
    // The bindings
    ColumnPtr * columns;

    QueryClass();

    QueryClass(const uint32_t &relId, const uint32_t &colCount, ColumnPtr * cols);

    void initialize(const uint32_t &relId, const uint32_t &colCount, ColumnPtr * cols);
};

typedef QueryClass* QueryPtr;

#endif
