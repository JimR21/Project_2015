#ifndef VALCLASS_HPP
#define VALCLASS_HPP
#include <inttypes.h>
#include "queryClass.hpp"

class ValClass
{
public:
    // The validation id. Monotonic increasing
    uint64_t validationId;
    // The transaction range
    uint64_t from,to;
    // The query count
    uint32_t queryCount;

    QueryPtr *queries;

    ValClass(const uint64_t &valId, const  uint64_t &ifrom, const uint64_t &ito, const uint32_t &qCount, QueryPtr * qs);

    ~ValClass();

    void sortByColumnCount();

    bool valOptimize();

#if VAL_HASHTABLE == 1
    bool valHashOptimize();
#endif

private:
    // Quicksort operations
    void quickSort(QueryPtr* A, const int &p, const int &q);
    int partition(QueryPtr* A, const int &p, const int &q);
};

#endif
