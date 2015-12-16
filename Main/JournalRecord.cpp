#include "JournalRecord.hpp"

using namespace std;

//============================================================
JournalRecord::JournalRecord(uint32_t tid, bool record_type){

    transaction_id = tid;
	type = record_type;
	values = new DArray<uint32_t>();
    // cout << "New Journal Record created" << endl;
}
//============================================================
void JournalRecord::addValue(uint32_t val){
    // cout << "Adding value: " << val << endl;
    values->push_back(val);
}
//============================================================
uint32_t JournalRecord::getValue(uint32_t key){
    return values->get(key);
}
bool JournalRecord::getType(){
	return type;
}
//============================================================
DArray<uint32_t>* JournalRecord::getAllValues(){
	return values;
}
//============================================================
uint32_t JournalRecord::getTransactionId(){
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
	delete values;
}
//============================================================
