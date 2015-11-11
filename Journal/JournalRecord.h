#include <iostream>
#include <stdint.h>
#include <vector>

using namespace std;

class JournalRecord {

private:
    uint64_t transaction_id;
    vector<uint64_t> values;

public:
    JournalRecord(uint64_t);
    void addValue(uint64_t);
    uint64_t getValue(uint64_t);
    uint64_t getTransactionId();
    void printRecord();
};
