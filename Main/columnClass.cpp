#include "columnClass.hpp"

using namespace std;


ColumnClass::ColumnClass(const Query::Column &col)
{
    op = col.op;
    column = col.column;
    value = col.value;
}

void ColumnClass::initialize(Query::Column col)
{
    op = col.op;
    column = col.column;
    value = col.value;
}

#if VAL_HASHTABLE == 1
ColumnClass::ColumnClass(const Query::Column &col, string &ikey)
{
    op = col.op;
    column = col.column;
    value = col.value;

    key = ikey;
}


void ColumnClass::initialize(Query::Column &col, string &ikey)
{
    op = col.op;
    column = col.column;
    value = col.value;

    key = ikey;
}

Bitset* ColumnClass::checkColumn(DArray<JournalRecord*> &records)
{
	Bitset *bitset;

	bool result;
    unsigned size = records.size();

	bitset = new Bitset(size/8+1);

	uint64_t tuple_value, query_value;
    for(unsigned i = 0; i < size; i++)
    {
        tuple_value = records.get(i)->getValue(column);
        query_value = value;
        switch (op)
        {
            case Equal:     result=(tuple_value == query_value); break;
            case NotEqual: 	result=(tuple_value != query_value); break;
            case Less: 		result=(tuple_value < query_value); break;
            case LessOrEqual: result=(tuple_value <= query_value); break;
            case Greater: 	result=(tuple_value > query_value); break;
            case GreaterOrEqual: result=(tuple_value >= query_value); break;
            default: result = false ; break;
        }
		if (result == true)
			bitset->setBitsetValue(i);
    }
    return bitset;
}
#endif
