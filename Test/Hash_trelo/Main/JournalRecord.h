#include <iostream>
#include <stdint.h>
#include "DArray.hpp"

class JournalRecord {

private:
    uint64_t transaction_id;
    DArray<uint64_t> *values;

public:
    JournalRecord(uint64_t);
    void addValue(uint64_t);
	uint64_t getValue(uint64_t);
	uint64_t getTransactionId();
    DArray<uint64_t> *getAllValues();
	~JournalRecord();
    void printRecord();
};
