#include "valClass.hpp"

using namespace std;

ValClass::ValClass(const uint64_t &valId, const  uint64_t &ifrom, const uint64_t &ito, const uint32_t &qCount, QueryPtr * qs) : validationId(valId), from(ifrom), to(ito), queryCount(qCount), queries(qs)
{

}

ValClass::~ValClass()
{
    for(unsigned i = 0; i < queryCount; i++)
        delete queries[i];

    delete[] queries;
}

void ValClass::quickSort(QueryPtr* A, const int &p, const int &q)
{
    int r;
    if(p < q)
    {
        r = partition(A, p,q);
        quickSort(A,p,r);
        quickSort(A,r+1,q);
    }
}

int ValClass::partition(QueryPtr* A, const int &p, const int &q)
{
    unsigned x = A[p]->columnCount;
    int i = p;

    for(int j = p + 1; j < q; j++)
        if(A[j]->columnCount <= x){
            i = i + 1;
            swap(A[i], A[j]);
        }

    swap(A[i],A[p]);
    return i;
}

void ValClass::sortByColumnCount()
{
    quickSort(queries, 0, queryCount);
}
