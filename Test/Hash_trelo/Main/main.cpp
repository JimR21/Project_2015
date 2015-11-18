#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>
#include "Journal.h"
#include "Bucket.hpp"
#include "HashTable.hpp"

using namespace std;

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

//=====================================================
//=================== FUNCTIONS =======================
//================================================================================================
static void processDefineSchema(DefineSchema *s){

	unsigned int i;

	cout << "DefineSchema " << s->relationCount << " |";

  	if (schema == NULL)
		free(schema);

  	schema = (uint32_t*)malloc(sizeof(uint32_t) * s->relationCount);	// allocate space for the relations
  	Journals = (Journal**)malloc(sizeof(Journal*) * s->relationCount); 	// allocate space for pointers to Journals
	hash_tables = (HashTable**)malloc(sizeof(HashTable*) * s->relationCount);

	for(i = 0; i < s->relationCount; i++){
    	cout << " " << s->columnCounts[i] << " ";	// print columns for every relation
		schema[i] = s->columnCounts[i];				// add # of columns for every relation to schema
	}

  	cout << endl;

  	cout << "=========================================================" << endl;
  	for(i = 0; i < s->relationCount; i++) {   	// For every relation
  		Journal* journal = new Journal(i);    	// Create empty Journal
		Journals[i] = journal;					// add Journal to Journals array
		hash_tables[i] = new HashTable();		// Create empty Hash for every rel
  	}
  	cout << "=========================================================" << endl;

	// exit(0);
}
//================================================================================================
static void processTransaction(Transaction *t){

    unsigned int i;
    const char* reader = t->operations;
    JournalRecord* record;

    cout << "Transaction " << t->transactionId << " (" << t->deleteCount << ", " << t->insertCount << ")" << " |";

	//=================================
	// Delete operations
	//=================================
    for(i = 0; i < t->deleteCount; i++) {
    	const TransactionOperationDelete* o = (TransactionOperationDelete*)reader;
        cout << "opdel rid " << o->relationId << " #rows " << o->rowCount << " ";

		cout << endl;
		cout << o->keys[0] << endl;

		//==========================================
		// TODO: LOOP GIA OLA TA ->KEYS[] TO DELETE
		//==========================================
        // unsigned index;
        // index = Journals[o->relationId]->searchRecord(o->keys[0]);    // search the Journal for record(c0) == key
		//
        // if (rec == NULL) {   // key not found, continue
		// 	reader += sizeof(TransactionOperationDelete) + (sizeof(uint64_t) * o->rowCount);
		// 	continue;
		// }
		//
        // record = new JournalRecord(t->transactionId);	// JournalRecord to be inserted
		//
        // for(unsigned int j = 0; j < schema[o->relationId]; j++)
        //     record->addValue(rec->getValue(j));  // copy the rest columns from the one found
		//
        // Journals[o->relationId]->insertJournalRecord(record);	// insert delete record to Journal

		// print offsets
		// cout << "ST: " << Journals[o->relationId]->getStartOffset() << " " << "END: " << Journals[o->relationId]->getEndOffset() <<  endl;

        // Go to the next delete operation
        reader += sizeof(TransactionOperationDelete) + (sizeof(uint64_t) * o->rowCount);
    }
    printf(" \t| ");
	//=================================
	// Insert Operations
	//=================================
    for(i = 0; i < t->insertCount; i++) {
        const TransactionOperationInsert* o = (TransactionOperationInsert*)reader;

        printf("opins rid %u #rows %u | ", o->relationId, o->rowCount);
    	printf("(");

		for(unsigned int j = 0; j< o->rowCount * schema[o->relationId]; j++) {	// iterate over values array
			if(j % schema[o->relationId] == 0) {		// start of group
				record = new JournalRecord(t->transactionId);
        	}
			printf("%lu ", o->values[j]);
        	record->addValue(o->values[j]);  // add value to record

			if((j + 1) % schema[o->relationId] == 0) {	// end of group
				printf(")");

				//===================================
				// Insert record to relation's journal
				//===================================
            	Journals[o->relationId]->insertJournalRecord(record);   // add record to relation's Journal

				int size = Journals[o->relationId]->getRecordsSize();
				cout << "=========================================================" << endl;
				cout << "Journal size: " << size << endl;
				cout << "Offset: " << size - 1 << endl;
				cout << "Tid: " << t->transactionId << endl;
				cout << "c0: " << record->getValue(0) << endl;
				cout << "=========================================================" << endl;
				//===================================
				// Update relation's hash
				//===================================
				hash_tables[o->relationId]->insert(record->getValue(0), t->transactionId, size - 1);

				if(j + 1 < o->rowCount * schema[o->relationId] ) printf("(");

				// print offsets
            	// cout << "ST: " << Journals[o->relationId]->getStartOffset() << " " << "END: " << Journals[o->relationId]->getEndOffset() <<  endl;
			}
		}
		// Go to the next insert operation
		reader+=sizeof(TransactionOperationInsert)+(sizeof(uint64_t)*o->rowCount*schema[o->relationId]);
    }

	Journals[1]->printJournal();	// print to journal gia na fainetai
	// int ret = hash_tables[1]->getLastJournalInsert(2);	// TEST1: den uparxei to 2
	int ret = hash_tables[1]->getLastJournalInsert(4);	// TEST2: uparxei to 4
	if (ret != -1)
    	cout << " c0 = " << 4 << " found!" << " Offset = " << ret << endl;

    printf("\n");

	exit(0);
}
//================================================================================================
static void processValidationQueries(ValidationQueries *v){
    cout << "ValidationQueries " << v->validationId << " [" << v->from << ", " << v->to << "] " << v->queryCount << endl;
}
//================================================================================================
static void processFlush(Flush *fl){
    cout << "Flush " << fl->validationId << endl;
}
//================================================================================================
static void processForget(Forget *fo){
    cout << "Forget " << fo->transactionId << endl;
}
//=====================================================
//================== MAIN PROGRAM =====================
//=====================================================
int main(int argc, char **argv) {

  MessageHead head;
  void *body = NULL;
  uint32_t len;

    while(1){
      // Retrieve the message head
      if (read(0, &head, sizeof(head)) <= 0) { return -1; } // crude error handling, should never happen
      printf("HEAD LEN %u \t| HEAD TYPE %u \t| DESC ", head.messageLen, head.type);

      // Retrieve the message body
      if (body != NULL) free(body);
      if (head.messageLen > 0 ){
          body = malloc(head.messageLen*sizeof(char));
          if (read(0, body, head.messageLen) <= 0) { printf("err");return -1; } // crude error handling, should never happen
          len-=(sizeof(head) + head.messageLen);
      }

      // And interpret it
      switch (head.type) {
         case MessageHead::Done: printf("DONE\n");
                    printf("Rel0\n");
                    Journals[0]->printJournal();
                    printf("Rel1\n");
                    Journals[1]->printJournal();
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
