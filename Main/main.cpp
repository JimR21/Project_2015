#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>
#include <string>
#include <chrono>
#include "Journal.hpp"
#include "Bucket.hpp"
#include "Key_HashTable.hpp"
#include "Tid_HashTable.hpp"
#include <fstream>

using namespace std;
using ns = chrono::milliseconds;
using get_time = chrono::steady_clock;

//Testing
unsigned transactionCounter = 0;
int termFlag = 0;

std::chrono::time_point<std::chrono::high_resolution_clock> default_time, globalstart, globalend, def_start, def_end, tran_start, tran_end, val_start, val_end, flush_start, flush_end, destroy_start, destroy_end;
std::chrono::duration<double> default_diff, globaldiff, def_diff, tran_diff, val_diff, flush_diff, destroy_diff;

ofstream myfile ("out.bin", ios::out);

//================================================================================================
struct MessageHead {
   	// Message types
   	enum Type : uint32_t { Done, DefineSchema, Transaction, ValidationQueries, Flush, Forget };
   	// Total message length, excluding this head
   	uint32_t messageLen;
   	// The message type
   	Type type;
};
//================================================================================================
struct DefineSchema {
   	// Number of relations
   	uint32_t relationCount;
   	// Column counts per relation, one count per relation. The first column is always the primary key
   	uint32_t columnCounts[];
};
//================================================================================================
struct Transaction {
   	// The transaction id. Monotonic increasing
   	uint64_t transactionId;
   	// The operation counts
   	uint32_t deleteCount,insertCount;
   	// A sequence of transaction operations. Deletes first, total deleteCount+insertCount operations
   	char operations[];
};
//================================================================================================
struct TransactionOperationDelete {
   	// The affected relation
   	uint32_t relationId;
   	// The row count
   	uint32_t rowCount;
   	// The deleted values, rowCount primary keys
   	uint64_t keys[];
};
//================================================================================================
struct TransactionOperationInsert {
   	// The affected relation
   	uint32_t relationId;
   	// The row count
   	uint32_t rowCount;
   	// The inserted values, rowCount * relation[relationId].columnCount values
   	uint64_t values[];
};
//================================================================================================
struct ValidationQueries {
   	// The validation id. Monotonic increasing
   	uint64_t validationId;
   	// The transaction range
   	uint64_t from,to;
   	// The query count
   	uint32_t queryCount;
   	// The queries
   	char queries[];
};
//================================================================================================
struct Query {
   	// A column description
	struct Column {
      // Support operations
      enum Op : uint32_t { Equal, NotEqual, Less, LessOrEqual, Greater, GreaterOrEqual };
      // The column id
      uint32_t column;
      // The operations
      Op op;
	  // The constant
	  uint64_t value;
   	};

   	// The relation
   	uint32_t relationId;
	// The number of bound columns
	uint32_t columnCount;
	// The bindings
	Column columns[];
};
//================================================================================================
struct Flush {
	// All validations to this id (including) must be answered
	uint64_t validationId;
};
//================================================================================================
struct Forget {
	// Transactions older than that (including) will not be tested for
	uint64_t transactionId;
};

static uint32_t* schema = NULL;  // keeps the # of columns for every relation
Journal** Journals = NULL;       // keeps the Journal for every relation
Key_HashTable** hash_tables;		 // Extendible hashing for every relation
DArray<bool> validationResults;	 // den xreiazetai new einai sto scope tis main
DArray<Query::Column>* subqueries_to_check;

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
	hash_tables = (Key_HashTable**)malloc(sizeof(Key_HashTable*) * s->relationCount);

    relationCount = s->relationCount;

	for(i = 0; i < s->relationCount; i++){
		schema[i] = s->columnCounts[i];				// add # of columns for every relation to schema
        if(schema[i] > maxColumnCounts) maxColumnCounts = schema[i];
	}

  	for(i = 0; i < s->relationCount; i++) {   	// For every relation
  		Journal* journal = new Journal(i);    	// Create empty Journal
		Journals[i] = journal;					// add Journal to Journals array
		hash_tables[i] = new Key_HashTable();		// Create empty Hash for every rel
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

			index = hash_tables[o->relationId]->getLastJournalInsert(o->keys[k]);

			if (index == -1){	// den to vrika
				continue;
			}
			else {

				// checking case of multiple deletes
				if (Journals[o->relationId]->getRecord(index)->getType() == DELETE){
					// Sunexomeno delete case
					continue;}

				// Else Delete after insertion

				record = new JournalRecord(t->transactionId, DELETE);	// JournalRecord to be inserted

				JournalRecord * jr = Journals[o->relationId]->getRecord(index);
				for(unsigned int j = 0; j < schema[o->relationId]; j++)
		            record->addValue(jr->getValue(j));  // copy the rest columns from the one found

				Journals[o->relationId]->insertJournalRecord(record);	// insert delete record to Journal

                int size = Journals[o->relationId]->getRecordsSize();

                // //===================================
				// // Update relation's tid hash
				// //===================================
                // if(tidflag)
                // {
                //     tid_hashtables[o->relationId]->insert(t->transactionId, size - 1);
                //     tidflag = false;
                // }


                //===================================
				// Update relation's key hash
				//===================================
				hash_tables[o->relationId]->insert(record->getValue(0), t->transactionId, size - 1);
			}
		}

        // Go to the next delete operation
        reader += sizeof(TransactionOperationDelete) + (sizeof(uint64_t) * o->rowCount);
    }
	//=================================
	// Insert Operations
	//=================================
    for(i = 0; i < t->insertCount; i++) {
        const TransactionOperationInsert* o = (TransactionOperationInsert*)reader;

		for(unsigned int j = 0; j< o->rowCount * schema[o->relationId]; j++) {	// iterate over values array
			if(j % schema[o->relationId] == 0) {		// start of group
				record = new JournalRecord(t->transactionId, INSERT);
        	}
        	record->addValue(o->values[j]);  // add value to record

			if((j + 1) % schema[o->relationId] == 0) {	// end of group
				//===================================
				// Insert record to relation's journal
				//===================================
            	Journals[o->relationId]->insertJournalRecord(record);   // add record to relation's Journal

                int size = Journals[o->relationId]->getRecordsSize();

                // //===================================
				// // Update relation's tid hash
				// //===================================
                // if(tidflag)
                // {
                //     tid_hashtables[o->relationId]->insert(t->transactionId, size - 1);
                //     tidflag = false;
                // }

				//===================================
				// Update relation's hash
				//===================================
				hash_tables[o->relationId]->insert(record->getValue(0), t->transactionId, size - 1);
			}
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

    val_start = std::chrono::high_resolution_clock::now();

	bool conflict = true;
	const char* reader = v->queries;

	// Iterate over the validation's queries
	for (unsigned i = 0; i != v->queryCount; i++)
    {
        conflict = true;
		const Query* q = (Query*)reader;

        // check an einai entos range
		uint64_t max_tid = Journals[q->relationId]->getLastTID();


		if (v->from > max_tid )	// mou dwse tid start pou einai megalutero tou max || keno query
        {
            // Go to the next query
            conflict = false;   // Se periptwsh poy den uparxei allo query
            reader += sizeof(Query)+(sizeof(Query::Column)*q->columnCount);
			continue;				// no need to check the next queries for this validation
        }

        if (q->columnCount == 0)    // an einai keno kai mesa sta oria tote conflict
        {
			conflict = true;
			break;
	    }


        DArray<Query::Column>* priority1 = new DArray<Query::Column>();        // subqueries me c0 =
        DArray<Query::Column>* priority2 = new DArray<Query::Column>();        // subqueries me =
        DArray<Query::Column>* priority3 = new DArray<Query::Column>();        // ola ta upoloipa


        //==========================================================
        // 1os elegxos: An einai valid ta predicates twn subqueries
        // Arxikopoihsh twn priority tables
        //==========================================================

		// iterate over subqueries
		for (unsigned w = 0; w < q->columnCount; w++)
        {
			// ean to operation einai '=' kai anaferetai sto primary key (c0)
			if (q->columns[w].op == Query::Column::Equal && q->columns[w].column == 0)
            {
                if(priority1->size() == 0)
                    priority1->push_back(q->columns[w]);
                else
                {
                    if(priority1->get(0).value != q->columns[w].value)  // An uparxei hdh subquery me diaforetikh timh sto c0 =
                    {
                        // Go to the next query
                        conflict = false;   // Se periptwsh poy den uparxei allo query
                        reader += sizeof(Query)+(sizeof(Query::Column)*q->columnCount);
            			break;				// no need to check the next queries for this validation
                    }
                }
            }
            else if(q->columns[w].op == Query::Column::Equal)
                priority2->push_back(q->columns[w]);
            else
                priority3->push_back(q->columns[w]);
		}

        if(conflict == false)   // Sthn periptvsh poy vgei false apo ton 1o elegxo
        {
            delete priority1;
            delete priority2;
            delete priority3;
            continue;
        }

        DArray<uint64_t>* offsets_to_check = NULL;      // DArray me ta offset pou exoyme na elegksoume
        DArray<JournalRecord*>* records_to_check = NULL;    // DArray me ta journal records poy exoume na elegksoume
        DArray<JournalRecord*>* records_to_check2 = new DArray<JournalRecord*>();   // Voithitikos DArray me records

        //================================================================
        // 2os elegxos: Filtrarisma twn records me vash ta priority tables
        //================================================================
        int prsize1 = priority1->size();
        for(int w = 0; w < prsize1; w++)
        {
            offsets_to_check = hash_tables[q->relationId]->getHashRecord(priority1->get(w).value, v->from, v->to);
            if(offsets_to_check != NULL)
            {
                records_to_check = new DArray<JournalRecord*>();
                for(int i = 0; i < offsets_to_check->size(); i++)
                    records_to_check->push_back(Journals[q->relationId]->getRecord(offsets_to_check->get(i)));
            }
            else
            {
                conflict = false;
                break;
            }
        }

        if(conflict == true)
        {
            int prsize2 = priority2->size();
            for(int w = 0; w < prsize2; w++)
            {
                if(records_to_check == NULL)
                    records_to_check = Journals[q->relationId]->getJournalRecords(v->from, v->to);

                int rtcsize = records_to_check->size();
                for(int i = 0; i < rtcsize; i++)
                {
                    if(priority2->get(w).value == records_to_check->get(i)->getValue(priority2->get(w).column))
                        records_to_check2->push_back(records_to_check->get(i));
                }
                if(records_to_check2->size() == 0)
                {
                    conflict = false;
                    break;
                }
                else
                {
                    delete records_to_check;
                    records_to_check = records_to_check2;
                    records_to_check2 = new DArray<JournalRecord*>();
                }
            }
        }

        if(conflict == true)
        {
            int prsize3 = priority3->size();
            for(int w = 0; w < prsize3; w++)
            {
                if(records_to_check == NULL)
                {
                    delete records_to_check;
                    records_to_check = Journals[q->relationId]->getJournalRecords(v->from, v->to);
                }
                int rtcsize = records_to_check->size();
                for(int i = 0; i < rtcsize; i++)
                {
                    uint64_t query_value = priority3->get(w).value;
                    uint64_t tuple_value = records_to_check->get(i)->getValue(priority3->get(w).column);
                    bool result;

                    switch (priority3->get(w).op)
                    {
                        case Query::Column::NotEqual: 	result=(tuple_value != query_value); break;
                        case Query::Column::Less: 		result=(tuple_value < query_value); break;
                        case Query::Column::LessOrEqual: result=(tuple_value <= query_value); break;
                        case Query::Column::Greater: 	result=(tuple_value > query_value); break;
                        case Query::Column::GreaterOrEqual: result=(tuple_value >= query_value); break;
                        default: result = false ; break;
            	    }

                    if(result == true)
                        records_to_check2->push_back(records_to_check->get(i));
                }
                if(records_to_check2->size() == 0)
                {
                    conflict = false;
                    break;
                }
                else
                {
                    delete records_to_check;
                    records_to_check = records_to_check2;
                    records_to_check2 = new DArray<JournalRecord*>();
                }
            }
        }


        delete offsets_to_check;
        delete records_to_check;
        delete records_to_check2;
        delete priority1;
        delete priority2;
        delete priority3;

        if(conflict == false)
        {
            // Go to the next query
            reader += sizeof(Query)+(sizeof(Query::Column)*q->columnCount);
        }
        else
            break;
    }


	// Store validation's conflict result
	validationResults.push_back(conflict);

    val_end = std::chrono::high_resolution_clock::now();
    if(val_diff != default_diff)
        val_diff = val_diff + val_end - val_start;
    else
        val_diff = val_end - val_start;
}
//================================================================================================
static void processFlush(Flush *fl){

    flush_start = std::chrono::high_resolution_clock::now();

    unsigned valRes_size = (unsigned)validationResults.size();
    unsigned i;

    string buf;

	for (i = val_offset; i < valRes_size && i<=fl->validationId; i++){
		//myfile << "Validation " << i << " : " << validationResults.get(i) << endl;
        buf.append(BoolToString(validationResults.get(i)));
	}

    cout << buf << endl;

    if(i >= fl->validationId)
	   val_offset = fl->validationId;
    else
        val_offset = valRes_size;

    cout << "Validation : " << val_offset << endl;
    cout << "TransactionCounter : " << transactionCounter << endl;

    flush_end = std::chrono::high_resolution_clock::now();
    if(flush_diff != default_diff)
        flush_diff = flush_diff + flush_end - flush_start;
    else
        flush_diff = flush_end - flush_start;

    // if(val_offset == 18090)
    // {
    //     termFlag = 1;
    // }
}
//================================================================================================
static void processForget(Forget *fo){}
//================================================================================================
static void processDestroySchema()
{
    destroy_start = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < relationCount; i++)
    {   	// For every relation
		delete Journals[i];
		delete hash_tables[i];
  	}
    free(schema);
    free(Journals);
    free(hash_tables);

    destroy_end = std::chrono::high_resolution_clock::now();
    destroy_diff = destroy_end - destroy_start;
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

        // if(termFlag == 1)
        //     head.type = MessageHead::Done;

		// And interpret it
		switch (head.type) {
			case MessageHead::Done:
                processDestroySchema();

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
