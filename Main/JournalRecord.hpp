#include <iostream>
#include <stdint.h>
#include "DArray.hpp"

#define DELETE false
#define INSERT true

class JournalRecord {

private:
    uint32_t transaction_id;
    DArray<uint32_t> *values;
	bool type;

public:
    JournalRecord(uint32_t, bool type);
	~JournalRecord();

	bool getType();
	uint32_t getValue(uint32_t);
	uint32_t getTransactionId();
	DArray<uint32_t> *getAllValues();

	void addValue(uint32_t);
	void printRecord();
};
