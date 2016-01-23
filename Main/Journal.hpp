#include <iostream>
#include <stdint.h>
#include "JournalRecord.hpp"
#include "Key_HashTable.hpp"
#include "Tid_HashTable.hpp"
#include "Val_HashTable.hpp"
#include "options.hpp"

class Journal {
private:
    //uint32_t relation_id;
    DArray<JournalRecord*> *Records;
    uint64_t lastTID;

#if TID_HASHTABLE == 1
    Tid_HashTable tid_hashtable;
#endif

public:
    Key_HashTable key_htable;
	Val_HashTable val_htable;
    uint32_t columns;

    Journal(uint32_t);
    ~Journal();
    void insertJournalRecord(JournalRecord*);
    int searchRecord(uint64_t);
	DArray<JournalRecord*> * getJournalRecords(uint64_t, uint64_t);	// returns array of arrays(JournalRecords)
    void printJournal();
	int getRecordsSize();
	JournalRecord *getRecordatOffset(int key);
    uint64_t getLastTID();
    DArray<JournalRecord*> * getRecordsbyKey(unsigned key, uint64_t start_tid, uint64_t end_tid);	// Uses Key_HashTable

#if TID_HASHTABLE == 1
    int tidSearchRecord(unsigned tid);
#endif
};
