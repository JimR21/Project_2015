#include <iostream>
#include <stdint.h>
#include <vector>
#include "JournalRecord.h"

using namespace std;

class Journal {

private:
    uint32_t relation_id;
    vector <JournalRecord*> Records;
    int start;  // start offset
    int end;    // end offset
    int prev_t_id;

public:
    Journal(uint32_t);
    bool insertJournalRecord(JournalRecord*);
    JournalRecord* searchRecord(uint64_t);
    void printJournal();
    int getStartOffset();
    int getEndOffset();
    void setStartOffset(int);
    void setEndOffset(int);
};
