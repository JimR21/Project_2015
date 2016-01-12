#include "JournalRecord.hpp"

using namespace std;

//============================================================
JournalRecord::JournalRecord(uint32_t columns){
	values = new uint64_t[columns];
    // cout << "New Journal Record created" << endl;
}
//============================================================
JournalRecord::JournalRecord(uint64_t tid, bool record_type, uint32_t columns){

    transaction_id = tid;
	type = record_type;
	values = new uint64_t[columns];
    // cout << "New Journal Record created" << endl;
}
//============================================================
JournalRecord::JournalRecord(const JournalRecord &rec, uint64_t newtid, uint32_t columns)
{
    transaction_id = newtid;
    type = DELETE;
    values = new uint64_t[columns];
    memcpy(values, rec.values, columns * sizeof(uint64_t));
}
//============================================================
void JournalRecord::addValue(uint64_t val, uint32_t pos){
    // cout << "Adding value: " << val << endl;
    values[pos] = val;
}
//============================================================
uint64_t JournalRecord::getValue(uint32_t pos){
    return values[pos];
}
bool JournalRecord::getType(){
	return type;
}
//============================================================
uint64_t* JournalRecord::getAllValues(){
	return values;
}
//============================================================
uint64_t JournalRecord::getTransactionId(){
    return transaction_id;
}
//============================================================
// void JournalRecord::printRecord(){
// 	for (int i = 0; i < values->size(); i++)
//         cout << values->get(i) << " ";
// 	cout << "\ttype = " << ((type == true) ? "INSERT" : "DELETE");
// 	cout << endl;
// }
//============================================================
JournalRecord::~JournalRecord(){
	delete[] values;
}
//============================================================
