#include <iostream>
#include <stdint.h>
#include "JournalRecord.h"


class Journal {

private:
    uint32_t relation_id;
    DArray<JournalRecord*> *Records;
    int start;  // start offset
    int end;    // end offset
    uint64_t prev_t_id;

public:
    Journal(uint32_t);
    void insertJournalRecord(JournalRecord*);
    JournalRecord* searchRecord(uint64_t);
	DArray<DArray<uint64_t>*> * getJournalRecords(uint64_t, uint64_t);	// returns array of arrays(JournalRecords)
    void printJournal();
	void destroyJournal();
    int getStartOffset();
    int getEndOffset();
    void setStartOffset(int);
    void setEndOffset(int);
	int getRecordsSize();
};
