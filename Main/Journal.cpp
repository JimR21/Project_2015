#include "Journal.hpp"

using namespace std;

//================================================================================================
Journal::Journal(uint32_t icolumns): columns(icolumns){
	Records = new DArray<JournalRecord*>();
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

#if TID_HASHTABLE == 1
    tid_hashtable.insert(lastTID, Records->size()-1);
#endif
}
//================================================================================================

DArray<JournalRecord*> * Journal::getJournalRecords(uint64_t start_tid, uint64_t end_tid){

	DArray<JournalRecord*> *recs = new DArray<JournalRecord*>();

	int idx = -1;

#if TID_HASHTABLE == 1
	while((idx = tidSearchRecord(start_tid)) == -1)	// psakse se pio index tou Journal tha ksekinisw na psaxnw
		start_tid += 1;								// an den yparxei to tid pou mou dwse san start psakse to epomeno
#else
    while((idx = searchRecord(start_tid)) == -1)	// psakse se pio index tou Journal tha ksekinisw na psaxnw
        start_tid += 1;								// an den yparxei to tid pou mou dwse san start psakse to epomeno
#endif

    int rsize = Records->size();
	for (int i = idx; i < rsize; i++){		// psakse siriaka apo ekei pou sou gurise i binary search
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
int Journal::getRecordsSize(){
	return Records->size();
}
//================================================================================================
JournalRecord* Journal::getRecordatOffset(int offset){
	return Records->get(offset);
}
//================================================================================================
uint64_t Journal::getLastTID()
{
    return lastTID;
}
//================================================================================================
#if TID_HASHTABLE == 1
int Journal::tidSearchRecord(unsigned tid){
    return tid_hashtable.getOffset(tid);
}
#endif
//================================================================================================
DArray<JournalRecord*> * Journal::getRecordsbyKey(unsigned key, uint64_t start_tid, uint64_t end_tid)
{
    DArray<unsigned>* offsets = key_htable.getHashRecords(key, start_tid, end_tid);

    if(offsets == NULL)
        return NULL;

    DArray<JournalRecord*> * records = new DArray<JournalRecord*>(100);

    unsigned size = offsets->size();
    for (unsigned i = 0; i < size; i++)
    {
        records->push_back(getRecordatOffset(offsets->get(i)));
    }

    delete offsets;
    return records;
}
//================================================================================================
#if VAL_HASHTABLE == 1
Bitset* Journal::getBitsetbyKey(unsigned key, uint64_t start_tid, uint64_t end_tid)
{
    DArray<unsigned>* offsets = key_htable.getHashRecords(key, start_tid, end_tid);
    unsigned start_offset, end_offset, i;

    if(offsets == NULL)
        return NULL;

#if TID_HASHTABLE == 1
	while((start_offset = tidSearchRecord(start_tid)) == -1)	// psakse se pio index tou Journal tha ksekinisw na psaxnw
        start_tid += 1;							// an den yparxei to tid pou mou dwse san start psakse to epomeno
    while((end_offset = tidSearchRecord(end_tid)) == -1)
        end_tid -= 1;

#else
    while((start_offset = searchRecord(start_tid)) == -1)	// psakse se pio index tou Journal tha ksekinisw na psaxnw
        start_tid += 1;								// an den yparxei to tid pou mou dwse san start psakse to epomeno
    while((end_offset = searchRecord(end_tid)) == -1)
        end_tid -= 1;
#endif

    int rsize = Records->size();
    for (i = end_offset; i < rsize; i++)
    {
        uint64_t tid = (Records->get(i))->getTransactionId();
        if (tid > end_tid)  	// an exw kseperasei to end_tid
            break;
    }

    end_offset = i;

    Bitset* bit = new Bitset((end_offset - start_offset)/8 + 1);

    for(i = 0; i < offsets->size(); i++)
    {
        bit->setBitsetValue(offsets->get(i) - start_offset);
    }

    delete offsets;
    return bit;
}
#endif
