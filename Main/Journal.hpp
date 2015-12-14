#include <iostream>
#include <stdint.h>
#include "JournalRecord.hpp"
#include "Tid_HashTable.hpp"

class Journal {
private:
    uint32_t relation_id;
    DArray<JournalRecord*> *Records;
    uint64_t lastTID;

    Tid_HashTable tid_hashtable;

public:
    Journal(uint32_t);
    ~Journal();
    void insertJournalRecord(JournalRecord*);
    int searchRecord(uint64_t);
	DArray<JournalRecord*> * getJournalRecords(uint64_t, uint64_t);	// returns array of arrays(JournalRecords)
    void printJournal();
	int getRecordsSize();
	JournalRecord *getRecord(int key);
    uint64_t getLastTID();

    int tidSearchRecord(unsigned tid);
};
