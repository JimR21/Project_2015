#ifndef COLUMNCLASS_HPP
#define COLUMNCLASS_HPP
#include <string>

#include "mainStructs.hpp"
#include "bitset.hpp"
#include "DArray.hpp"
#include "JournalRecord.hpp"


class ColumnClass
{
public:
    // The column id
    uint32_t column;
    // The operations
    Op op;
    // The constant
    uint64_t value;

#if VAL_HASHTABLE == 1
    // Val_HashTable
    std::string key;
#endif

    ColumnClass(const Query::Column &col);

    void initialize(Query::Column);

#if VAL_HASHTABLE == 1
    ColumnClass(const Query::Column &col, std::string &key);

    void initialize(Query::Column &col, std::string &key);

    Bitset* checkColumn( DArray<JournalRecord*> &records);
#endif
};

typedef ColumnClass* ColumnPtr;

#endif
