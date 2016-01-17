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
	JournalRecord *getRecord(int key);
    uint64_t getLastTID();

	int countRecordsInRange(uint64_t from, uint64_t to);	// posa records uparxoun se auto to range (gia to bitset)

#if TID_HASHTABLE == 1
    int tidSearchRecord(unsigned tid);
#endif
};
