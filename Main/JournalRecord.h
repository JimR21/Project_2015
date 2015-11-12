#include <iostream>
#include <stdint.h>
#include "DArray.h"

class JournalRecord {

private:
    uint64_t transaction_id;
    DArray* values;

public:
    JournalRecord(uint64_t);
    void addValue(uint64_t);
    uint64_t getValue(uint64_t);
    uint64_t getTransactionId();
    void printRecord();
};
