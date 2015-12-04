#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>
#include <chrono>
#include "Journal.hpp"
#include "Bucket.hpp"
#include "HashTable.hpp"
#include <fstream>

using namespace std;
using ns = chrono::seconds;
using get_time = chrono::steady_clock;

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
HashTable** hash_tables;		 // Extendible hashing for every relation
DArray<bool> validationResults;	 // den xreiazetai new einai sto scope tis main
DArray<Query::Column>* subqueries_to_check;


int val_offset = 0;
uint32_t maxColumnCounts = 0;
//=====================================================
//=================== FUNCTIONS =======================
//================================================================================================
static void processDefineSchema(DefineSchema *s){

	unsigned int i;

	// cout << "DefineSchema " << s->relationCount << " |";

  	if (schema == NULL)
		free(schema);

  	schema = (uint32_t*)malloc(sizeof(uint32_t) * s->relationCount);	// allocate space for the relations
  	Journals = (Journal**)malloc(sizeof(Journal*) * s->relationCount); 	// allocate space for pointers to Journals
	hash_tables = (HashTable**)malloc(sizeof(HashTable*) * s->relationCount);

	for(i = 0; i < s->relationCount; i++){
    	// cout << " " << s->columnCounts[i] << " ";	// print columns for every relation
		schema[i] = s->columnCounts[i];				// add # of columns for every relation to schema
        if(schema[i] > maxColumnCounts) maxColumnCounts = schema[i];
	}

 //  	cout << endl;

  	for(i = 0; i < s->relationCount; i++) {   	// For every relation
  		Journal* journal = new Journal(i);    	// Create empty Journal
		Journals[i] = journal;					// add Journal to Journals array
		hash_tables[i] = new HashTable();		// Create empty Hash for every rel
  	}


}
//================================================================================================
static void processTransaction(Transaction *t){

    unsigned i;
	int index;
    const char* reader = t->operations;
    JournalRecord* record;

    if(t->transactionId == 753)
    {
        cout << "tid" << endl;
    }


    // cout << "Transaction " << t->transactionId << " (" << t->deleteCount << ", " << t->insertCount << ")" << " |" << endl;
	// cout << "=====================================================================" << endl;


	//=================================
	// Delete operations
	//=================================
    for(i = 0; i < t->deleteCount; i++) {
    	const TransactionOperationDelete* o = (TransactionOperationDelete*)reader;
		// cout << endl;
		// cout << "--------------------------------------------" << endl;
        // cout << "opdel rid " << o->relationId << " #rows " << o->rowCount << " " << endl;
		// cout << "--------------------------------------------" << endl;

		for (unsigned k = 0; k < o->rowCount; k++){
			// cout << "Searching for primary key: " << o->keys[k] << endl;

			index = hash_tables[o->relationId]->getLastJournalInsert(o->keys[k]);

			if (index == -1){	// den to vrika
				continue;
			}
			else {

				// checking case of multiple deletes
				if (Journals[o->relationId]->getRecord(index)->getType() == DELETE){
					// cout << "Sunexomeno delete case" << endl;
					continue;}
				else
					// cout << "Delete after insertion" << endl;

				// cout << endl;
				record = new JournalRecord(t->transactionId, DELETE);	// JournalRecord to be inserted

				JournalRecord * jr = Journals[o->relationId]->getRecord(index);
				for(unsigned int j = 0; j < schema[o->relationId]; j++)
		            record->addValue(jr->getValue(j));  // copy the rest columns from the one found

				Journals[o->relationId]->insertJournalRecord(record);	// insert delete record to Journal
                //===================================
				// Update relation's hash
				//===================================
                int size = Journals[o->relationId]->getRecordsSize();
				hash_tables[o->relationId]->insert(record->getValue(0), t->transactionId, size - 1);
			}
		}

        // Go to the next delete operation
        reader += sizeof(TransactionOperationDelete) + (sizeof(uint64_t) * o->rowCount);
    }
    // printf(" \t| ");
	//=================================
	// Insert Operations
	//=================================
    for(i = 0; i < t->insertCount; i++) {
        const TransactionOperationInsert* o = (TransactionOperationInsert*)reader;

		// cout << endl;
		// cout << "--------------------------------------------" << endl;
        // printf("opins rid %u #rows %u | \n", o->relationId, o->rowCount);
		// cout << "--------------------------------------------" << endl;

		for(unsigned int j = 0; j< o->rowCount * schema[o->relationId]; j++) {	// iterate over values array
			if(j % schema[o->relationId] == 0) {		// start of group
				record = new JournalRecord(t->transactionId, INSERT);
				// printf("(");
        	}
			// printf("%lu ", o->values[j]);
        	record->addValue(o->values[j]);  // add value to record

			if((j + 1) % schema[o->relationId] == 0) {	// end of group
				// printf(")");

				//===================================
				// Insert record to relation's journal
				//===================================
            	Journals[o->relationId]->insertJournalRecord(record);   // add record to relation's Journal

				int size = Journals[o->relationId]->getRecordsSize();
				// cout << "=========================================================" << endl;
				// cout << "Journal size: " << size << endl;
				// cout << "Offset: " << size - 1 << endl;
				// cout << "Tid: " << t->transactionId << endl;
				// cout << "c0: " << record->getValue(0) << endl;
				// cout << "=========================================================" << endl;
				//===================================
				// Update relation's hash
				//===================================
				hash_tables[o->relationId]->insert(record->getValue(0), t->transactionId, size - 1);

				// if(j + 1 < o->rowCount * schema[o->relationId] ) printf("(");
			}
		}
		// Go to the next insert operation
		reader+=sizeof(TransactionOperationInsert)+(sizeof(uint64_t)*o->rowCount*schema[o->relationId]);
    }

    // printf("\n");

}
//================================================================================================
static void processValidationQueries(ValidationQueries *v){

	// cout << "ValidationQueries " << v->validationId << " [" << v->from << ", " << v->to << "] " << v->queryCount << endl;
	// cout << "=====================================================================" << endl;

    if(v->validationId == 4792)
    {
        cout << "ok" << endl;
    }


	// DArray<DArray<unsigned>>* array;	// array me ta rangearrays gia to case c0=<x>
	bool conflict = true;
    //bool no_conflict = false;   // Sigouro oti den uparxei conflict gia ta subqueries

	const char* reader = v->queries;

	// Iterate over the validation's queries
	for (unsigned i = 0; i != v->queryCount; i++)
    {
        conflict = true;
		const Query* q = (Query*)reader;

        // if(v->validationId == 2782 && q->relationId == 5)
        // {
        //     Journals[5]->printJournal();
        // }

		// adeio validation
		if (v->queryCount == 1 && q->columnCount == 0){
			conflict = true;
			break;
	       }

		int idx = (Journals[q->relationId]->getRecordsSize())-1;	// check an einai entos range
		JournalRecord *jt = Journals[q->relationId]->getRecord(idx);
		uint64_t max_tid = jt->getTransactionId();


		if (v->from > max_tid || q->columnCount == 0)	// mou dwse tid start pou einai megalutero tou max || keno query
        {
            // Go to the next query
            conflict = false;   // Se periptwsh poy den uparxei allo qu
            reader += sizeof(Query)+(sizeof(Query::Column)*q->columnCount);
			continue;				// no need to check the next queries for this validation
        }

        //Journals[q->relationId]->printJournal();

        DArray<Query::Column>* priority1 = new DArray<Query::Column>();        // subqueries me c0 =
        DArray<Query::Column>* priority2 = new DArray<Query::Column>();        // subqueries me =
        DArray<Query::Column>* priority3 = new DArray<Query::Column>();        // ola ta upoloipa

        DArray<Query::Column> tempCol[maxColumnCounts];
		// iterate over subqueries
		for (unsigned w = 0; w < q->columnCount; w++)
        {
			// cout << "Subquery~> c" << q->columns[w].column << " , operator = " << q->columns[w].op << " , val: " << q->columns[w].value << endl;

            tempCol[q->columns[w].column].push_back(q->columns[w]);

			// ean to operation einai '=' kai anaferetai sto primary key (c0)
			if (q->columns[w].op == Query::Column::Equal && q->columns[w].column == 0)
            {
                if(priority1->size() == 0)
                    priority1->push_back(q->columns[w]);
                else
                {
                    if(priority1->get(0).value != q->columns[w].value)
                    {
                        // Go to the next query
                        conflict = false;   // Se periptwsh poy den uparxei allo qu
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

        if(conflict == false)
            continue;

        // Elegxos an ena column emfanizetai panw apo 1 fora sto idio query
        // for (unsigned w = 0; w < q->columnCount; w++)
        // {
        //
        // }

        DArray<uint64_t>* offsets_to_check = NULL;
        DArray<JournalRecord*>* records_to_check = NULL;
        DArray<JournalRecord*>* records_to_check2 = new DArray<JournalRecord*>();

        for(int w = 0; w < priority1->size(); w++)
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
            for(int w = 0; w < priority2->size(); w++)
            {
                if(records_to_check == NULL)
                    records_to_check = Journals[q->relationId]->getJournalRecords(v->from, v->to);

                for(int i = 0; i < records_to_check->size(); i++)
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
                    if(w < priority2->size() - 1)
                    {
                        delete records_to_check;
                        records_to_check = records_to_check2;
                        records_to_check2 = new DArray<JournalRecord*>();
                    }
                }
            }

        if(priority2->size() > 0)
        {
            delete records_to_check;
            records_to_check = records_to_check2;
            records_to_check2 = new DArray<JournalRecord*>();
        }

        if(conflict == true)
            for(int w = 0; w < priority3->size(); w++)
            {
                if(records_to_check == NULL)
                {
                    delete records_to_check;
                    records_to_check = Journals[q->relationId]->getJournalRecords(v->from, v->to);
                }

                for(int i = 0; i < records_to_check->size(); i++)
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
                        case Query::Column::Equal: 		result=(tuple_value == query_value); break;
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

        delete records_to_check;
        records_to_check = records_to_check2;
        records_to_check2 = NULL;

        if(offsets_to_check != NULL)
            delete offsets_to_check;
        if(records_to_check != NULL)
            delete records_to_check;
        if(records_to_check2 != NULL)
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
        {
            break;
        }
    }


	// Store validation's conflict result
	validationResults.push_back(conflict);
}
//================================================================================================
static void processFlush(Flush *fl){

    // myfile << "Flush " << fl->validationId << endl;
	// cout << "=====================================================================" << endl;

    unsigned valRes_size = (unsigned)validationResults.size();
    unsigned i;

	for (i = val_offset; i < valRes_size && i<=fl->validationId; i++){
		myfile << "Validation " << i << " : " << validationResults.get(i) << endl;
	}
    if(i >= fl->validationId)
	   val_offset = fl->validationId;
    else
        val_offset = valRes_size;
	//
	// cout << "New offset: " << val_offset << endl;
	// exit(0);
}
//================================================================================================
static void processForget(Forget *fo){
	// cout << "Forget " << fo->transactionId << endl;
}
//================================================================================================
static void processDestroySchema()
{
    int i = 0;
    while(schema[i] != 0) {   	// For every relation
		delete Journals[i];
		delete hash_tables[i];
        i++;
  	}
    free(schema);
    free(Journals);
    free(hash_tables);
}

//=====================================================
//================== MAIN PROGRAM =====================
//=====================================================
int main(int argc, char **argv) {

	MessageHead head;
	void *body = NULL;
	uint32_t len;


    auto start = get_time::now();
    auto end = get_time::now();
    auto diff = end - start;
    while(1){
		// Retrieve the message head
		if (read(0, &head, sizeof(head)) <= 0) { return -1; } // crude error handling, should never happen
		// cout << "=====================================================================" << endl;
     //  	printf("HEAD LEN %u \t| HEAD TYPE %u \t| DESC ", head.messageLen, head.type);

		// Retrieve the message body
		if (body != NULL) free(body);
		if (head.messageLen > 0 ){
			body = malloc(head.messageLen*sizeof(char));
			if (read(0, body, head.messageLen) <= 0) { printf("err");return -1; } // crude error handling, should never happen
			len-=(sizeof(head) + head.messageLen);
		}

		// And interpret it
		switch (head.type) {
			case MessageHead::Done: //printf("DONE\n");
				// cout << "=====================================================================" << endl;
				// printf("Rel0\n");
				// Journals[23]->printJournal();
				// printf("Rel1\n");
				// Journals[1]->printJournal();
				//========= TEST AN DOULEVEI H GETJOURNALRECORDS =============
				// DArray<DArray<uint64_t>*> * arr;
				// arr = Journals[1]->getJournalRecords(0,3);
				// for (int w = 0; w < arr->size(); w++){
				// 	for (int k = 0; k < (arr->get(w))->size(); k++)
				// 		cout << (arr->get(w))->get(k) << " ";
				//
				// 	cout <<endl;
				// }
				// cout << Journals[0]->Records->size();
				//========= TEST AN DOULEVEI H DESTROYJOURNAL =============
				// Journals[1]->destroyJournal();

                processDestroySchema();

                end = get_time::now();
                diff = end - start;
                cout << "Elapsed time is :  " << chrono::duration_cast<ns>(diff).count() << " ms " << endl;

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
