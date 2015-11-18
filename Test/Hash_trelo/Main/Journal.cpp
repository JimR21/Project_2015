#include "Journal.h"

using namespace std;

Journal::Journal(uint32_t rel_id){
    relation_id = rel_id;
    start = -1;
    end = -1;
    prev_t_id = -1;
	Records = new DArray<JournalRecord*>();
    cout << "Created New Journal for Relation: " << relation_id << endl;
}

void Journal::insertJournalRecord(JournalRecord* record){
    Records->push_back(record);
    if( prev_t_id != record->getTransactionId() )
        start = end = end + 1;
    else
        end++;
    prev_t_id = record->getTransactionId();
}

DArray<DArray<uint64_t>*> * Journal::getJournalRecords(uint64_t start_tid, uint64_t end_tid){

	DArray<DArray<uint64_t>*> *recs = new DArray<DArray<uint64_t>*>();

	for (int i = 0; i < Records->size(); i++){

		uint64_t tid = (Records->get(i))->getTransactionId();	// tid of the specific JournalRecord

		if (tid >= start_tid && tid <= end_tid)	{// filter
			DArray<uint64_t> *arr = (Records->get(i))->getAllValues();
			recs->push_back(arr); }
	}
	return recs;
}

JournalRecord* Journal::searchRecord(uint64_t key){

    int start = 0;  			// start of the array
    int end = Records->size() - 1;  // end of the array

	int m;

    while (start <= end){
		m = (start + end) / 2;	// middle index
		cout << "Start: " << start << ", End: " << end << ", mid: " << m << endl;

        uint64_t cur_val = (Records->get(m))->getValue(0);
		cout << "Cur val: " << cur_val << ", Key: " << key << endl;
        if (cur_val == key){     // found it
            return Records->get(m);
        }
        else if (cur_val < key)
            start = m + 1;
        else
            end = m - 1;
    }
    return NULL;
}

void Journal::printJournal() {
	for(int i = 0; i < Records->size(); i++){
        cout << (Records->get(i))->getTransactionId() << " ";
        (Records->get(i))->printRecord();
    }
}

void Journal::destroyJournal(){
	cout << "Deleting Journal" << endl;
	for (int i = 0; i < Records->size(); i++){	// destroy all of its JournalRecords
		delete Records->get(i);
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

int Journal::getRecordsSize(){
	return Records->size();
}
