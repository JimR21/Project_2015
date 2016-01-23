#ifndef COLUMNCLASS_HPP
#define COLUMNCLASS_HPP
#include "mainStructs.hpp"
#include <string>

class ColumnClass
{
public:
    // The column id
    uint32_t column;
    // The operations
    Op op;
    // The constant
    uint64_t value;

    // Val_HashTable
    std::string key;

    ColumnClass(const Query::Column &col);

    ColumnClass(const Query::Column &col, std::string &key);

    void initialize(Query::Column);

    void initialize(Query::Column &col, std::string &key);
};

typedef ColumnClass* ColumnPtr;

#endif
