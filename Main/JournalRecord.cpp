#include "JournalRecord.h"
using namespace std;

JournalRecord::JournalRecord(uint64_t tid){

	values = new DArray();	// katse ti gamato einai to darray
    transaction_id = tid;
    cout << "New Journal Record created" << endl;
}
void JournalRecord::addValue(uint64_t val){
    cout << "Adding value: " << val << endl;
    values->push_back(val);
}
uint64_t JournalRecord::getValue(uint64_t key){
    return values->get(key);
}
uint64_t JournalRecord::getTransactionId(){
    return transaction_id;
}

void JournalRecord::printRecord(){
    for (int i = 0; i < values->size(); i++){
        cout << values->get(i) << " ";
    }
    cout << endl;
}
