#include <iostream>
#include <stdint.h>
#include "JournalRecord.hpp"


class Journal {

private:
    uint32_t relation_id;
    DArray<JournalRecord*> *Records;

public:
    Journal(uint32_t);
    void insertJournalRecord(JournalRecord*);
    int searchRecord(uint64_t);
	DArray<DArray<uint64_t>*> * getJournalRecords(uint64_t, uint64_t);	// returns array of arrays(JournalRecords)
    void printJournal();
	void destroyJournal();
	int getRecordsSize();
	JournalRecord *getRecord(int key);
};
