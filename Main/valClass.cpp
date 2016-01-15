#include "valClass.hpp"

ValClass::ValClass(const uint64_t &valId, const  uint64_t &ifrom, const uint64_t &ito, const uint32_t &qCount, QueryPtr * qs) : validationId(valId), from(ifrom), to(ito), queryCount(qCount), queries(qs)
{

}
