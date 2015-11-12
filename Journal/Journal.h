#include <iostream>
#include <stdint.h>
#include "JournalRecord.h"


class Journal {

private:
    uint32_t relation_id;
    DArray* Records;
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
