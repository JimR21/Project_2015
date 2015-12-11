#include "JournalRecord.h"

using namespace std;

//============================================================
JournalRecord::JournalRecord(uint64_t tid, bool record_type){

    transaction_id = tid;
	type = record_type;
	values = new DArray<uint64_t>();
    // cout << "New Journal Record created" << endl;
}
//============================================================
void JournalRecord::addValue(uint64_t val){
    // cout << "Adding value: " << val << endl;
    values->push_back(val);
}
//============================================================
uint64_t JournalRecord::getValue(uint64_t key){
    return values->get(key);
}
bool JournalRecord::getType(){
	return type;
}
//============================================================
DArray<uint64_t>* JournalRecord::getAllValues(){
	return values;
}
//============================================================
uint64_t JournalRecord::getTransactionId(){
    return transaction_id;
}
//============================================================
void JournalRecord::printRecord(){
	for (int i = 0; i < values->size(); i++)
        cout << values->get(i) << " ";
	cout << "\ttype = " << ((type == true) ? "INSERT" : "DELETE");
	cout << endl;
}
//============================================================
JournalRecord::~JournalRecord(){
	cout << "Destroying JournalRecord" << endl;
	delete values;
}
//============================================================
