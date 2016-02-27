#include <chrono>
#include <string>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>
#include "Bucket.hpp"
#include "valClass.hpp"
#include "Threads/Thread.hpp"

#define ROUNDS 1		// Default = 1

unsigned flush_counter = 0;

using namespace std;


void setBitsetValue(int index, char *array){
	int bit_index = index / CHAR_BIT;		// which char
	int bit_number = index % CHAR_BIT;		// which bit of this char

	array[bit_index] = array[bit_index] | (1 << (7 - bit_number));	// update !!
}

bool valOptimize(ValClass *v);      // Part 1 Optimizations
bool valHashOptimize(ValClass *v);
unsigned forget = 0;

using ns = chrono::milliseconds;
using get_time = chrono::steady_clock;

//Testing
int termFlag = 0;
int predicates = 0;
unsigned transactionCounter = 0;
unsigned found = 0, totalin = 0;

std::chrono::time_point<std::chrono::high_resolution_clock> default_time, globalstart, globalend, def_start, def_end, tran_start, tran_end, val_start, val_end, flush_start, flush_end, destroy_start, destroy_end;
std::chrono::duration<double> default_diff, globaldiff, def_diff, tran_diff, val_diff, flush_diff, destroy_diff;

ofstream myfile ("out.bin", ios::out);
static uint32_t* schema = NULL;  // keeps the # of columns for every relation
Journal** Journals = NULL;       // keeps the Journal for every relation
DArray<bool> validationResults;	 // den xreiazetai new einai sto scope tis main
DArray<Query::Column>* subqueries_to_check;

DArray<bool> resultValidationList;

#if VAL_THREADS == 1
	DArray<ValidationNode*> validationList('a','a');

	pthread_mutex_t counter_mutex;
	pthread_cond_t  counter_cv;
#else
	DArray<ValidationNode*> validationList;
#endif

unsigned jobs = 0;
// DArray<Val_listbucket*> *jobs;

int relationCount = 0;
int val_offset = 0;
uint32_t maxColumnCounts = 0;

unsigned lastFlushId = 0;

//=====================================================
//=================== FUNCTIONS =======================
//=====================================================

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
		schema[i] = s->columnCounts[i];		// add # of columns for every relation to schema
        if(schema[i] > maxColumnCounts)
			maxColumnCounts = schema[i];
	}

    for(i = 0; i < s->relationCount; i++)    			// For every relation
  		Journals[i] = new Journal(s->columnCounts[i]);  // Create empty Journal

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
				if (Journals[o->relationId]->getRecordatOffset(index)->getType() == DELETE)
					continue;		// Sunexomeno delete case


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

	bool flag = false;

    const char* reader = v->queries;
	int size = sizeof(ValidationQueries) * v->queryCount ;

    QueryPtr * queries = new QueryPtr[v->queryCount];       // Pinakas apo deiktes se

	// iterate over queries of this validation
	for (unsigned i = 0; i != v->queryCount; i++)
    {
		const Query* q = (Query*)reader;

        ColumnPtr * columns = new ColumnPtr[q->columnCount];

		//=============================================================
		//=============================================================
		//=============================================================

        // check an einai entos range
        uint64_t max_tid = Journals[q->relationId]->getLastTID();

		if ((v->from < max_tid) && q->columnCount == 0){	// an keno query kai entos range
            flag = true; // vrika keno kai entos
        }
		//=============================================================
		//=============================================================
		//=============================================================

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

	//=============================================================
	//=============================================================
	ValidationNode *validation = new ValidationNode(val);

	if (flag)	// vrika true query
		validation->setResult(flag);	// kanw olo to val true
	//=============================================================
	//=============================================================

	// Add validation
  	validationList.push_back(validation);


    val_end = std::chrono::high_resolution_clock::now();
    if(val_diff != default_diff)
        val_diff = val_diff + val_end - val_start;
    else
        val_diff = val_end - val_start;
}
//================================================================================================
bool printValidationsUntilFlush(DArray<bool>* resultValidationList,uint64_t validationId){
	unsigned i = 0;

	// oso exw pragmata sto resultValidationList mou
	while (resultValidationList->size() != 0){

		if(validationId - lastFlushId < i)	// an vrika valID > tou flush val ID vges
		   return true;

		// cout << "Validation " << lastFlushId + i << " : " << resultValidationList->getLast() << endl;	// alliws tupwse to apotelesma
		resultValidationList->popLast();
		i++;
	}
	lastFlushId += i;
	return false;
}
//================================================================================================
void validateAndMove(DArray<ValidationNode*>* validationList,DArray<bool>* resultValidationList){

	while (validationList->size()!=0){

		ValidationNode* validationNode = validationList->getLast();
		ValClass* val = validationNode->getValidation();
		bool conflict = false;

		if (validationNode->getResult() == false) {	// an den exei vrethei true apo panw

			#if VAL_THREADS == 1	// sto thread case apla tha kaneis tin metafora apo val se res
									// ta vals einai idi ipologismena apo ta threads
			#elif VAL_HASHTABLE == 1
				conflict = valHashOptimize(val);
				validationNode->setResult(conflict);
			#else
				conflict = valOptimize(val);
				validationNode->setResult(conflict);
			#endif
		}

		resultValidationList->push_back(validationNode->getResult());

		delete validationNode;
		validationList->popLast();

		// edw tha borousame sto thread case na kanoume copy to valList sto resultValidationList
		// kai delete meta olon ton valList anti gia ena ena pop
	}

}
//================================================================================================
static void processFlush(Flush *fl){

#if ROUNDS > 1
	if(flush_counter < ROUNDS)
	{
		flush_counter++;
		return;
	}
	else
		flush_counter = 1;
#endif

	flush_start = std::chrono::high_resolution_clock::now();

	#if VAL_THREADS == 1

		jobs = validationList.size();	// jobs = osa validations exw gia ypologismo

		///////////////////////////////////
		validationList.resetIndex();	// reset to index tou array gia to epomeno paketo validations
		///////////////////////////////////

		// exoume ta validations sto queue. mas irthe to flush ara ksipname
		// ola ta worker threads gia na arxisoun na travane apo to validationList queue
		validationList.mainFlag = true;
		validationList.wakeUpWorkers();


		pthread_mutex_lock(&counter_mutex);	// lock counter
			while (jobs > 0){
				pthread_cond_wait(&counter_cv, &counter_mutex);	// release mutex & wait till the condition is signaled
			}
			validationList.mainFlag = false;
			// cout << "Workers are done" << endl;
	    pthread_mutex_unlock(&counter_mutex);	// unlock counter
	#endif



	bool result = printValidationsUntilFlush(&resultValidationList,fl->validationId);

	if (!result){
	  	validateAndMove(&validationList,&resultValidationList);
		printValidationsUntilFlush(&resultValidationList,fl->validationId);
	}


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

	for(int i = 0; i < validationList.size(); i++)
		delete validationList.get(i);

    destroy_end = std::chrono::high_resolution_clock::now();
    destroy_diff = destroy_end - destroy_start;
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
            conflict = false;  	 	// Se periptwsh poy den uparxei allo query
            continue;				// no need to check the next queries for this validation
        }

        if (q->columnCount == 0)    // an einai keno το skiparw
            continue;

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
            continue;

        conflict = q->hashValidate(Journals[q->relationId], v->from, v->to);

		if(conflict == true)
			break;
    }

    return conflict;
}
#endif

//=======================================================================

//=====================================================
//================== MAIN PROGRAM =====================
//=====================================================
int main(int argc, char **argv) {

    globalstart = std::chrono::high_resolution_clock::now();

	MessageHead head;
	void *body = NULL;
	uint32_t len;

	#if VAL_THREADS == 1

		pthread_mutex_init(&counter_mutex, NULL);	// initialize mutex
		pthread_cond_init(&counter_cv, NULL);		// initialize cond var

		for (int i = 0; i < NUM_OF_THREADS; i++){
			Thread *thread = new Thread(validationList);
			thread->start();
		}

	#endif

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

                // processDestroySchema();

                globalend = std::chrono::high_resolution_clock::now();
                globaldiff = globalend - globalstart;

              cout << "Define schema elapsed time is :  " << def_diff.count() << " s " << endl;
              cout << "Transactions elapsed time is :  " << tran_diff.count() << " s " << endl;
              cout << "Validations elapsed time is :  " << val_diff.count() << " s " << endl;
              cout << "Flush elapsed time is :  " << flush_diff.count() << " s " << endl;
              cout << "Destroy elapsed time is :  " << destroy_diff.count() << " s " << endl;
              cout << "Overal elapsed time is :  " << globaldiff.count() << " s " << endl;

			  	#if VAL_THREADS == 1
					pthread_mutex_destroy(&counter_mutex);
					pthread_cond_destroy(&counter_cv);
			    #endif
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
