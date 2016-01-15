#include "columnClass.hpp"

using namespace std;

ColumnClass::ColumnClass()
{

}

ColumnClass::ColumnClass(const Query::Column &col)
{
    op = col.op;
    column = col.column;
    value = col.value;
}


ColumnClass::ColumnClass(Query::Column &col, string &ikey)
{
    op = col.op;
    column = col.column;
    value = col.value;

    key = ikey;
}

void ColumnClass::initialize(Query::Column col)
{
    op = col.op;
    column = col.column;
    value = col.value;
}

void ColumnClass::initialize(Query::Column &col, string &ikey)
{
    op = col.op;
    column = col.column;
    value = col.value;

    key = ikey;
}
