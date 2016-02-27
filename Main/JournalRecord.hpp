#ifndef JOURNALRECORD_HPP
#define JOURNALRECORD_HPP
#include <iostream>
#include <stdint.h>
#include "DArray.hpp"

#define DELETE false
#define INSERT true

class JournalRecord {

private:
    uint64_t transaction_id;
    uint64_t* values;
	bool type;

public:
    JournalRecord(uint32_t);
    JournalRecord(uint64_t, bool, uint32_t);
	~JournalRecord();

    JournalRecord(const JournalRecord &rec, uint64_t newtid, uint32_t columns);   // Copy constractor for DELETE records

	bool getType();
	uint64_t getValue(uint32_t);
	uint64_t getTransactionId();
	uint64_t*getAllValues();

	void addValue(uint64_t, uint32_t);
	void printRecord();
};
#endif
