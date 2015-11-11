#include "Journal.h"


Journal::Journal(uint32_t rel_id){
    relation_id = rel_id;
    start = -1;
    end = -1;
    prev_t_id = -1;
    cout << "Created New Journal for Relation: " << relation_id << endl;
}

bool Journal::insertJournalRecord(JournalRecord* record){
    Records.push_back(record);
    if( prev_t_id != record->getTransactionId() )
        start = end = end + 1;
    else
        end++;
    prev_t_id = record->getTransactionId();
}


JournalRecord* Journal::searchRecord(uint64_t key){

    int start = 0;  // start of the array
    int end = Records.size();   // end of the array

    int m = (start + end) / 2;
    while (start != end){
        uint64_t cur_val = Records[m]->getValue(0);
        if (cur_val == key){     // found it
            return Records[m];
        }
        else if (cur_val > key)
            end = m - 1;
        else
            start = m + 1;
        m = (start + end) / 2;
    }
    return NULL;
}

void Journal::printJournal() {
    for(int i=0;i<Records.size();i++){
        cout << Records[i]->getTransactionId() << " ";
        Records[i]->printRecord();
    }
}

int Journal::getStartOffset(){
    return start; }

int Journal::getEndOffset(){
    return end; }

void Journal::setStartOffset(int s){
    start = s; }

void Journal::setEndOffset(int e){
    end = e; }
