#include <iostream>
#include <stdint.h>
#include "DArray.hpp"
#include <fstream>
#include <iomanip>


#define DELETE false
#define INSERT true

class JournalRecord {

private:
    uint64_t transaction_id;
    DArray<uint64_t> *values;
	bool type;

public:
    JournalRecord(uint64_t, bool type);
	~JournalRecord();

	bool getType();
	uint64_t getValue(uint64_t);
	uint64_t getTransactionId();
	DArray<uint64_t> *getAllValues();

	void addValue(uint64_t);
	void printRecord(std::ofstream& jout);
};
