#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string>
#include <chrono>
//#include "Journal.hpp"
#include "Bucket.hpp"
#include "ValidationIndex.hpp"
#include "valClass.hpp"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <limits.h>
#include <pthread.h>
#define NUM_OF_THREADS 5  //me 2 threads petaei seg fault

using namespace std;

void printBitset(char c){
	int i;

    for (i = 7; i >= 0; --i)
        putchar( (c & (1 << i)) ? '1' : '0' );

    putchar('|');
}
void setBitsetValue(int index, char *array){
	int bit_index = index / CHAR_BIT;		// which char
	int bit_number = index % CHAR_BIT;		// which bit of this char

	array[bit_index] = array[bit_index] | (1 << (7 - bit_number));	// update !!
}

// operator for query sort
struct sorting
{
    bool operator() (const QueryPtr & lhs, const QueryPtr & rhs) { return lhs->relationId < rhs->relationId; }
} sorting_by_relationId;


string stringBuilder(int start, int end, int col, int op, uint64_t value);
bool valOptimize(ValClass *v);      // Part 1 Optimizations
bool valHashOptimize(ValClass *v);
//void threadExecuteValidations(DArray<ValClass*>* validationArray);
char* checkColumn(ColumnPtr c, DArray<JournalRecord*> * records);
unsigned forget = 0;

using ns = chrono::milliseconds;
using get_time = chrono::steady_clock;

//Testing
unsigned found = 0, totalin = 0;
int predicates = 0;
unsigned transactionCounter = 0;
int termFlag = 0;

std::chrono::time_point<std::chrono::high_resolution_clock> default_time, globalstart, globalend, def_start, def_end, tran_start, tran_end, val_start, val_end, flush_start, flush_end, destroy_start, destroy_end;
std::chrono::duration<double> default_diff, globaldiff, def_diff, tran_diff, val_diff, flush_diff, destroy_diff;

ofstream myfile ("out.bin", ios::out);
static uint32_t* schema = NULL;  // keeps the # of columns for every relation
Journal** Journals = NULL;       // keeps the Journal for every relation
//Key_HashTable** hash_tables;		 // Extendible hashing for every relation
DArray<bool> validationResults;	 // den xreiazetai new einai sto scope tis main
DArray<Query::Column>* subqueries_to_check;

ValidationIndex valIndex;

int relationCount = 0;
int val_offset = 0;
uint32_t maxColumnCounts = 0;

//=====================================================
//=================== FUNCTIONS =======================
//=====================================================
inline const char * const BoolToString(bool b)
{
    return b ? "1" : "0";
}
//==============================================================================================
static void processDefineSchema(DefineSchema *s){

    def_start = std::chrono::high_resolution_clock::now();

	unsigned int i;

  	if (schema == NULL)
		free(schema);

  	schema = (uint32_t*)malloc(sizeof(uint32_t) * s->relationCount);	// allocate space for the relations
    Journals = (Journal**)malloc(sizeof(Journal*) * s->relationCount); 	// allocate space for pointers to Journals

    relationCount = s->relationCount;

	for(i = 0; i < s->relationCount; i++){
		schema[i] = s->columnCounts[i];				// add # of columns for every relation to schema
        if(schema[i] > maxColumnCounts) maxColumnCounts = schema[i];
	}

    for(i = 0; i < s->relationCount; i++) {   	// For every relation
  		Journals[i] = new Journal(s->columnCounts[i]);    	// Create empty Journal
		//hash_tables[i] = new Key_HashTable();		// Create empty Hash for every rel
    }

    def_end = std::chrono::high_resolution_clock::now();
    def_diff = def_end - def_start;

}
//================================================================================================
static void processTransaction(Transaction *t){

    tran_start = std::chrono::high_resolution_clock::now();

    unsigned i;
	int index;
    const char* reader = t->operations;
    JournalRecord* record;

    transactionCounter = t->transactionId;

    //bool tidflag = true;

	//=================================
	// Delete operations
	//=================================
    for(i = 0; i < t->deleteCount; i++) {
    	const TransactionOperationDelete* o = (TransactionOperationDelete*)reader;

		for (unsigned k = 0; k < o->rowCount; k++){
			// cout << "Searching for primary key: " << o->keys[k] << endl;

			index = Journals[o->relationId]->key_htable.getLastJournalInsert(o->keys[k]);

			if (index == -1){	// den to vrika
				continue;
			}
			else {

				// checking case of multiple deletes
				if (Journals[o->relationId]->getRecordatOffset(index)->getType() == DELETE){
					// Sunexomeno delete case
					continue;}

				// Else Delete after insertion

                JournalRecord * jr = Journals[o->relationId]->getRecordatOffset(index);
                record = new JournalRecord(*jr, t->transactionId, Journals[o->relationId]->columns);

				Journals[o->relationId]->insertJournalRecord(record);	// insert delete record to Journal

                int size = Journals[o->relationId]->getRecordsSize();

                //===================================
				// Update relation's key hash
				//===================================
				Journals[o->relationId]->key_htable.insert(record->getValue(0), t->transactionId, size - 1);
			}
		}

        // Go to the next delete operation
        reader += sizeof(TransactionOperationDelete) + (sizeof(uint64_t) * o->rowCount);
    }
	//=================================
	// Insert Operations
	//=================================
    uint32_t k = 0;
    for(i = 0; i < t->insertCount; i++) {
        const TransactionOperationInsert* o = (TransactionOperationInsert*)reader;

		for(unsigned int j = 0; j< o->rowCount * schema[o->relationId]; j++) {	// iterate over values array
			if(j % schema[o->relationId] == 0) {		// start of group
				record = new JournalRecord(t->transactionId, INSERT, Journals[o->relationId]->columns);
                k = 0;
        	}
        	record->addValue(o->values[j], k);  // add value to record

			if((j + 1) % schema[o->relationId] == 0) {	// end of group
				//===================================
				// Insert record to relation's journal
				//===================================
            	Journals[o->relationId]->insertJournalRecord(record);   // add record to relation's Journal

                int size = Journals[o->relationId]->getRecordsSize();

				//===================================
				// Update relation's hash
				//===================================
				Journals[o->relationId]->key_htable.insert(record->getValue(0), t->transactionId, size - 1);
			}
            k++;
		}
		// Go to the next insert operation
		reader+=sizeof(TransactionOperationInsert)+(sizeof(uint64_t)*o->rowCount*schema[o->relationId]);
    }

    tran_end = std::chrono::high_resolution_clock::now();

    if(tran_diff != default_diff)
        tran_diff = tran_diff + tran_end - tran_start;
    else
        tran_diff = tran_end - tran_start;
}
//================================================================================================
static void processValidationQueries(ValidationQueries *v){

	// cout << "Val Id " << v->validationId << endl;
    const char* reader = v->queries;
	int size = sizeof(ValidationQueries) * v->queryCount ;

    QueryPtr * queries = new QueryPtr[v->queryCount];       // Pinakas apo deiktes se

	// iterate over queries of this validation
	for (unsigned i = 0; i != v->queryCount; i++)
    {
		const Query* q = (Query*)reader;

        ColumnPtr * columns = new ColumnPtr[q->columnCount];

		// calculate total records in range for the bitset allocation
		// int records_in_range = Journals[q->relationId]->countRecordsInRange(v->from, v->to);

        // iterate over subqueries of this query
		for (unsigned w = 0; w < q->columnCount; w++)
        {
			#if VAL_HASHTABLE == 1
				/////////////////////////
	            // Part 2: Val_HashTable
	            /////////////////////////
				// create the key for the validation's hash
				string key = stringBuilder(v->from, v->to, q->columns[w].column, q->columns[w].op, q->columns[w].value);
				Journals[q->relationId]->val_htable.insert(key, 0);	// predicate to val hash
				columns[w] = new ColumnClass(q->columns[w], key);
			#else
				columns[w] = new ColumnClass(q->columns[w]);
			#endif
        }

        queries[i] = new QueryClass(q->relationId, q->columnCount, columns);

        size += (sizeof(Query) + sizeof(Query::Column) * q->columnCount);
		reader += sizeof(Query)+(sizeof(Query::Column)*q->columnCount);
	}

    ValClass* val = new ValClass(v->validationId, v->from, v->to, v->queryCount, queries);

	// Add validation
	valIndex.insertValidation(val);

    val_end = std::chrono::high_resolution_clock::now();
    if(val_diff != default_diff)
        val_diff = val_diff + val_end - val_start;
    else
        val_diff = val_end - val_start;
}
//================================================================================================
void* threadExecuteValidations(void* parameterArray){                                          //this function call every thread to execute its validation list
	DArray<Val_listbucket*>* validationArray=(DArray<Val_listbucket*>*)parameterArray;
	for(unsigned i=0;i<validationArray->size();i++){
		bool conflict = valOptimize(validationArray->get(i)->getVal());
		validationArray->get(i)->setResult(conflict);
	}
	pthread_exit(NULL);

}
//================================================================================================
static void processFlush(Flush *fl){

    flush_start = std::chrono::high_resolution_clock::now();

	#if VAL_THREADS == 1
		void* status;
		int rc;
		DArray<Val_listbucket*> *threadArrays[NUM_OF_THREADS];              //create array for every thread
		for(unsigned i=0;i<NUM_OF_THREADS;i++){
			threadArrays[i]=new DArray<Val_listbucket*>();
		}
		pthread_t threads[NUM_OF_THREADS];                                  //create threads
	#endif


    unsigned size = valIndex.getSize();

    // flush ws to validationId i an den exei tosa, mexri to telos tis listas
    for (unsigned i = 0; i < size && i < fl->validationId; i++)
    {

		#if VAL_THREADS == 1
			/////////////////////////
			// Part 3: Val Threads
			/////////////////////////
			threadArrays[(valIndex.getNextValidation()->getVal()->validationId)%3]->push_back(valIndex.getNextValidation());  //every thread take its validation with logic of mod that was descriped on part3
			valIndex.fakePopValidation();                          //like popvalidation but dont delete validations it only moves the pointer
		#elif VAL_HASHTABLE == 1
	        /////////////////////////
	        // Part 2: Val_HashTable
	        /////////////////////////
			// Get validation to calculate
	    	ValClass *v = valIndex.getHeadValidation();
			bool conflict = valHashOptimize(v);
			valIndex.popValidation();
		#else
			/////////////////////////
			// Part 1: Optimizations
			/////////////////////////
			// Get validation to calculate
	    	ValClass *v = valIndex.getHeadValidation();
			bool conflict = valOptimize(v);
			valIndex.popValidation();
		#endif
    }


	#if VAL_THREADS == 1
		/////////////////////////
		// Part 3: Val Threads
		/////////////////////////
		for(unsigned i=0;i<NUM_OF_THREADS;i++)
		{
		    //cout <<"i : "<<i<<endl;
			rc = pthread_create(&threads[i], NULL,threadExecuteValidations,(void*)threadArrays[i]);        //every thread execute its array

			if (rc){
				cout << "Error:unable to create thread," << rc << endl;
				exit(-1);
			}
		}

		for(unsigned i=0; i < NUM_OF_THREADS; i++ ){                     //main thread wait for subthreads to complete their computation
			rc = pthread_join(threads[i], &status);
			if (rc){
				cout << "Error:unable to join," << rc << endl;
				exit(-1);
			}
			//cout << "Main: completed thread id :" << i ;
			//cout << "  exiting with status :" << status << endl;
		}

		for (unsigned i = 0; i < size && i < fl->validationId; i++){    //print result that have been saved on validation index
			ValClass *v = valIndex.getHeadValidation();
			//cout<<valIndex.getHeadBucket()->getResult();
			valIndex.popValidation();
		}
	#endif


    flush_end = std::chrono::high_resolution_clock::now();
    if(flush_diff != default_diff)
        flush_diff = flush_diff + flush_end - flush_start;
    else
        flush_diff = flush_end - flush_start;
}
//================================================================================================
static void processForget(Forget *fo)
{
	forget = fo->transactionId;
}
//================================================================================================
static void processDestroySchema()
{
    destroy_start = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < relationCount; i++) // For every relation
		delete Journals[i];
    free(schema);
    free(Journals);

    destroy_end = std::chrono::high_resolution_clock::now();
    destroy_diff = destroy_end - destroy_start;
}
//================================================================================================
string stringBuilder(int start, int end, int col, int op, uint64_t value){
    string key;

    key = to_string(start) + "-" + to_string(end) + "c" + to_string(col);

    switch(op){
        case Equal: key += "="; break;
        case NotEqual: key += "!"; break;
        case Less: key += "<"; break;
        case LessOrEqual: key += "["; break;
        case Greater: key += ">"; break;
        case GreaterOrEqual: key += "]"; break;
    }

    key += to_string(value);

    return key;
}
//================================================================================================
bool valOptimize(ValClass *v)
{
    bool conflict;
	
    // sort queries by column count
	v->sortByColumnCount();

    // Iterate over the validation's queries
    for (unsigned i = 0; i != v->queryCount; i++)
    {
        conflict = true;
        const QueryPtr q = v->queries[i];

        // check an einai entos range
        uint64_t max_tid = Journals[q->relationId]->getLastTID();
        if (v->from > max_tid )	// mou dwse tid start pou einai megalutero tou max || keno query
        {
            // Go to the next query
            conflict = false;   // Se periptwsh poy den uparxei allo query
            continue;				// no need to check the next queries for this validation
        }

        if (q->columnCount == 0)    // an einai keno kai mesa sta oria tote conflict
        {
            conflict = true;
            break;
        }

        conflict = q->validate(*Journals[q->relationId], v->from, v->to);

		if(conflict == true)
			break;
    }
    return conflict;
}
//=======================================================================
#if VAL_HASHTABLE == 1
bool valHashOptimize(ValClass * v)
{
	int bitset_size;
	int current_rel = -1;
    bool conflict = false;

	Bitset *resultBitset = NULL;

	DArray<JournalRecord*> *recs = NULL;

	// sort queries by column count
	std::sort(v->queries, v->queries + v->queryCount, sorting());

	// for every query of this validation
    for(unsigned i = 0; i < v->queryCount; i++)
    {
        QueryPtr q = v->queries[i];

		//=============================
		// check an einai entos range
		//=============================
		uint64_t max_tid = Journals[q->relationId]->getLastTID();

		if (v->from > max_tid )	// mou dwse tid start pou einai megalutero tou max || keno query
            continue;			// Go to the next query

		//=========================
		// empty query ~> conflict
		//=========================
		if (q->columnCount == 0){
			conflict = true; break;}

		if (current_rel == -1 || current_rel != q->relationId){
			delete recs;
			recs = Journals[q->relationId]->getJournalRecords(v->from, v->to);
			current_rel = q->relationId;
		}

		// for every subquery of this query
        for (unsigned w = 0; w < q->columnCount; w++)
        {
            ColumnPtr c = q->columns[w];
			conflict = false;

			// check if bitset is calculated
			int counter;

            Bitset* bitset = Journals[q->relationId]->val_htable.getbdata(c->key, &counter);
            if(bitset == NULL)	// not calculated case
            {
				bitset_size = recs->size();

            	bitset = Journals[q->relationId]->val_htable.UpdateValData(c->key, checkColumn(c, recs), bitset_size);

            }

			if (w == 0)	// an eimai sto 1o predicate
				resultBitset = new Bitset(*bitset);
			else		// an eimai sta epomena predicates apla kane tin praksi
				for (int j = 0; j < bitset->getSize(); j++)
					(resultBitset->getBitsetArray())[j] = (resultBitset->getBitsetArray())[j] & (bitset->getBitsetArray())[j];

			// Delete
			if((counter == 0) && (forget > v->from)){
				// cout << "Deleting in val: " << v->validationId << endl;
				Journals[q->relationId]->val_htable.deleteKey(c->key);
			}
			for (int j = 0; j < resultBitset->getSize(); j++)
				if ((resultBitset->getBitsetArray())[j] != 0){
					conflict = true;
					break;
				}
			if(conflict == false)
				break;
        }

		delete resultBitset;

		if(conflict == true)
		{
			delete recs;
			break;
		}
		if(i == v->queryCount - 1)
			delete recs;
    }
	return conflict;
}
#endif
//=======================================================================
char* checkColumn(ColumnPtr c, DArray<JournalRecord*> * records)
{
	char *bitset;

	bool result;
    unsigned size = records->size();

	// DArray<bool>* bitset = new DArray<bool>(size);
	bitset = (char*)malloc(size/8 + 1);

	for (unsigned i = 0; i < size/8 + 1; i++)
		bitset[i] = 0;

	uint64_t tuple_value, query_value;
    for(unsigned i = 0; i < size; i++)
    {
        tuple_value = records->get(i)->getValue(c->column);
        query_value = c->value;
        switch (c->op)
        {
            case Equal:     result=(tuple_value == query_value); break;
            case NotEqual: 	result=(tuple_value != query_value); break;
            case Less: 		result=(tuple_value < query_value); break;
            case LessOrEqual: result=(tuple_value <= query_value); break;
            case Greater: 	result=(tuple_value > query_value); break;
            case GreaterOrEqual: result=(tuple_value >= query_value); break;
            default: result = false ; break;
        }
		if (result == true)
			setBitsetValue(i, bitset);
    }
	// printBitset(*bitset);
	// cout << endl;
    return bitset;
}
//=====================================================
//================== MAIN PROGRAM =====================
//=====================================================
int main(int argc, char **argv) {

    globalstart = std::chrono::high_resolution_clock::now();

	MessageHead head;
	void *body = NULL;
	uint32_t len;

    while(1){
		// Retrieve the message head
		if (read(0, &head, sizeof(head)) <= 0) { return -1; } // crude error handling, should never happen

		// Retrieve the message body
		if (body != NULL) free(body);
		if (head.messageLen > 0 ){
			body = malloc(head.messageLen*sizeof(char));
			if (read(0, body, head.messageLen) <= 0) { printf("err");return -1; } // crude error handling, should never happen
			len-=(sizeof(head) + head.messageLen);
		}

        if(termFlag == 1)
            head.type = MessageHead::Done;

		// And interpret it
		switch (head.type) {
			case MessageHead::Done:

                // Debuging
                // cout << "============= HashTable Sizes ==============" << endl;
                // cout << "No  | Key HashTable    |   Val HashTable" << endl;
                // for(int i = 0; i < relationCount; i++)
                // {
                //     cout << setw(3) << left << i << " | Size: " << setw(8) << left << Journals[i]->key_htable.getsize() <<  " Inserts: " << setw(8) << left << Journals[i]->key_htable.inserts <<  " | Size: "  << setw(8) << left << Journals[i]->val_htable.getsize() << setw(8) << left << " Inserts: " << Journals[i]->val_htable.inserts << endl;
                // }
                //
                // cout << "Total Val_HashTable inserts: " << totalin << endl;
                // cout << "Times same predicate found: " << found << " (" << (double)found/totalin*100 << "\%)" << endl;



                //processDestroySchema();

                globalend = std::chrono::high_resolution_clock::now();
                globaldiff = globalend - globalstart;

               cout << "Define schema elapsed time is :  " << def_diff.count() << " ms " << endl;
               cout << "Transactions elapsed time is :  " << tran_diff.count() << " ms " << endl;
               cout << "Validations elapsed time is :  " << val_diff.count() << " ms " << endl;
               cout << "Flush elapsed time is :  " << flush_diff.count() << " ms " << endl;
               cout << "Destroy elapsed time is :  " << destroy_diff.count() << " ms " << endl;
               cout << "Overal elapsed time is :  " << globaldiff.count() << " ms " << endl;


				return 0;
			case MessageHead::DefineSchema: processDefineSchema((DefineSchema *)body); break;
			case MessageHead::Transaction: processTransaction((Transaction *)body); break;
			case MessageHead::ValidationQueries: processValidationQueries((ValidationQueries *)body); break;
			case MessageHead::Flush: processFlush((Flush *)body); break;
			case MessageHead::Forget: processForget((Forget *)body); break;
         	default: return -1; // crude error handling, should never happen
      	}
	}
    return 0;
}
