#include "queryClass.hpp"

QueryClass::QueryClass()
{

}

QueryClass::QueryClass(const uint32_t &relId, const uint32_t &colCount, ColumnPtr * cols)
{
    relationId = relId;
    columnCount = colCount;
    columns = cols;
}

void QueryClass::initialize(const uint32_t &relId, const uint32_t &colCount, ColumnPtr * cols)
{
    relationId = relId;
    columnCount = colCount;
    columns = cols;
}
