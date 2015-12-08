#include "Journal.hpp"

using namespace std;

//================================================================================================
Journal::Journal(uint32_t rel_id){
    relation_id = rel_id;
	Records = new DArray<JournalRecord*>();
    // cout << "Created New Journal for Relation: " << relation_id << endl;
}
//================================================================================================
Journal::~Journal(){
	for (int i = 0; i < Records->size(); i++)	// destroy all of its JournalRecords
		delete Records->get(i);

	delete Records;
}
//================================================================================================
void Journal::insertJournalRecord(JournalRecord* record){
    Records->push_back(record);
    lastTID = record->getTransactionId();
}
//================================================================================================

DArray<JournalRecord*> * Journal::getJournalRecords(uint64_t start_tid, uint64_t end_tid){

	DArray<JournalRecord*> *recs = new DArray<JournalRecord*>();

	int idx = -1;
													// ipothetw oti den tha dwsei range ektos oriwn
	while((idx = searchRecord(start_tid)) == -1)	// psakse se pio index tou Journal tha ksekinisw na psaxnw
		start_tid += 1;								// an den yparxei to tid pou mou dwse san start psakse to epomeno

	// cout << "To range ksekinaei sto index: " << idx << endl;

	for (int i = idx; i < Records->size(); i++){	// psakse siriaka apo ekei pou sou gurise i binary search
													// mexri to end_tid
		uint64_t tid = (Records->get(i))->getTransactionId();	// tid of the specific JournalRecord

		if (tid <= end_tid)	{	// oso den exw kseperasei to end_tid
			recs->push_back(Records->get(i));
		}
		else
			break;
	}
	return recs;
}
//================================================================================================
int Journal::searchRecord(uint64_t key){

	// cout << "Searching for transaction id: " << key << endl;
    int start = 0;  				// start of the array
    int end = Records->size() - 1;  // end of the array
	int m;
	uint64_t cur_val;
	uint64_t cur_val2;

	while (start <= end){
		m = (start + end) / 2;

		cur_val = (Records->get(m))->getTransactionId();
		cur_val2 = (Records->get(m-1))->getTransactionId();
		if (cur_val == key){	// if element is found
			if (m > 0 && cur_val == cur_val2)	// check if it is the first occurence
				end = m - 1;
			else
				return m;	//we found the first occurence return it.
		}
		else if (cur_val < key)
			start = m + 1;
		else
			end = m - 1;
	}
	return -1;
}
//================================================================================================
void Journal::printJournal() {
	for(int i = 0; i < Records->size(); i++){
        cout << (Records->get(i))->getTransactionId() << " ";
        (Records->get(i))->printRecord();
    }
}
//================================================================================================
int Journal::getRecordsSize(){
	return Records->size();
}
//================================================================================================
JournalRecord* Journal::getRecord(int offset){
	return Records->get(offset);
}
//================================================================================================
uint64_t Journal::getLastTID()
{
    return lastTID;
}
