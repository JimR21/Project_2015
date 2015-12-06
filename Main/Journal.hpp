#include <iostream>
#include <stdint.h>
#include "JournalRecord.hpp"


class Journal {

private:
    uint32_t relation_id;
    DArray<JournalRecord*> *Records;
    uint64_t lastTID;

public:
    Journal(uint32_t);
    void insertJournalRecord(JournalRecord*);
    int searchRecord(uint64_t);
	DArray<JournalRecord*> * getJournalRecords(uint64_t, uint64_t);	// returns array of arrays(JournalRecords)
    void printJournal();
	void destroyJournal();
	int getRecordsSize();
	JournalRecord *getRecord(int key);
    uint64_t getLastTID();
};
