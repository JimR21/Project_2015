#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string>
#include <chrono>
#include "Journal.hpp"
#include "Bucket.hpp"
#include "ValidationIndex.hpp"
#include "valClass.hpp"
#include <fstream>
#include <iomanip>
#include <algorithm>

using namespace std;

void printBitset(char c){
	int i;

    for (i = 7; i >= 0; --i)
        putchar( (c & (1 << i)) ? '1' : '0' );

    putchar('|');
}


// operator for query sort
struct sorting
{
    bool operator() (const QueryPtr & lhs, const QueryPtr & rhs) { return lhs->relationId < rhs->relationId; }
} sorting_by_relationId;


string stringBuilder(int start, int end, int col, int op, uint64_t value);
bool valOptimize(ValClass *v);      // Part 1 Optimizations
bool valHashOptimize(ValClass *v);
DArray<bool>* checkColumn(ColumnPtr c, DArray<JournalRecord*> * records);

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
				if (Journals[o->relationId]->getRecord(index)->getType() == DELETE){
					// Sunexomeno delete case
					continue;}

				// Else Delete after insertion

                JournalRecord * jr = Journals[o->relationId]->getRecord(index);
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
            /////////////////////////
            // Part 2: Val_HashTable
            /////////////////////////
			// create the key for the validation's hash
			string key = stringBuilder(v->from, v->to, q->columns[w].column, q->columns[w].op, q->columns[w].value);
            if(Journals[q->relationId]->val_htable.getbdata(key) != NULL)
                found++;
            else
            {
			    Journals[q->relationId]->val_htable.insert(key, 0);	// predicate to val hash
                totalin++;
            }

            columns[w] = new ColumnClass(q->columns[w],key);
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
static void processFlush(Flush *fl){

	// cout << "FLUSH" << endl;
    flush_start = std::chrono::high_resolution_clock::now();

    unsigned size = valIndex.getSize();

    // flush ws to validationId i an den exei tosa, mexri to telos tis listas
    for (unsigned i = 0; i < size && i < fl->validationId; i++)
    {
        // Get validation to calculate
    	ValClass *v = valIndex.getHeadValidation();

        /////////////////////////
        // Part 1: Optimizations
        /////////////////////////
        // bool conflict = valOptimize(v);

        /////////////////////////
        // Part 2: Val_HashTable
        /////////////////////////
		bool conflict = valHashOptimize(v);

        cout << "Validation " << v->validationId << " : " << conflict << endl;
        valIndex.popValidation();
    }


    flush_end = std::chrono::high_resolution_clock::now();
    if(flush_diff != default_diff)
        flush_diff = flush_diff + flush_end - flush_start;
    else
        flush_diff = flush_end - flush_start;
}
//================================================================================================
static void processForget(Forget *fo){}
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

    key = to_string(start) + "-" + to_string(end) + "@" + to_string(col);

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

        DArray<ColumnPtr>* priority1 = new DArray<ColumnPtr>(10);        // subqueries me c0 =
        DArray<ColumnPtr>* priority2 = new DArray<ColumnPtr>(10);        // subqueries me =
        DArray<ColumnPtr>* priority3 = new DArray<ColumnPtr>(10);        // ola ta upoloipa

        //==========================================================
        // 1os elegxos: An einai valid ta predicates twn subqueries
        // Arxikopoihsh twn priority tables
        //==========================================================

        // iterate over subqueries
        for (unsigned w = 0; w < q->columnCount; w++)
        {
            // ean to operation einai '=' kai anaferetai sto primary key (c0)
            if (q->columns[w]->op == Equal && q->columns[w]->column == 0)
            {
                if(priority1->size() == 0)
                    priority1->push_back(q->columns[w]);
                else
                {
                    if(priority1->get(0)->value != q->columns[w]->value)  // An uparxei hdh subquery me diaforetikh timh sto c0 =
                    {
                        // Go to the next query
                        conflict = false;   // Se periptwsh poy den uparxei allo query
                        break;				// no need to check the next queries for this validation
                    }
                }
            }
            else if(q->columns[w]->op == Equal)
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
        DArray<JournalRecord*>* records_to_check2 = new DArray<JournalRecord*>(100);   // Voithitikos DArray me records

        //================================================================
        // 2os elegxos: Filtrarisma twn records me vash ta priority tables
        //================================================================
        int prsize1 = priority1->size();
        for(int w = 0; w < prsize1; w++)
        {
            offsets_to_check = Journals[q->relationId]->key_htable.getHashRecord(priority1->get(w)->value, v->from, v->to);
            if(offsets_to_check != NULL)
            {
                records_to_check = new DArray<JournalRecord*>(100);
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
                    if(priority2->get(w)->value == records_to_check->get(i)->getValue(priority2->get(w)->column))
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
                    records_to_check2 = new DArray<JournalRecord*>(100);
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
                    uint64_t query_value = priority3->get(w)->value;
                    uint64_t tuple_value = records_to_check->get(i)->getValue(priority3->get(w)->column);
                    bool result;

                    switch (priority3->get(w)->op)
                    {
                        case NotEqual: 	result=(tuple_value != query_value); break;
                        case Less: 		result=(tuple_value < query_value); break;
                        case LessOrEqual: result=(tuple_value <= query_value); break;
                        case Greater: 	result=(tuple_value > query_value); break;
                        case GreaterOrEqual: result=(tuple_value >= query_value); break;
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
                    records_to_check2 = new DArray<JournalRecord*>(100);
                }
            }
        }

        delete offsets_to_check;
        delete records_to_check;
        delete records_to_check2;
        delete priority1;
        delete priority2;
        delete priority3;

        if(conflict == true)
            break;
    }
    return conflict;
}
//=======================================================================
bool valHashOptimize(ValClass * v)
{
	int bitset_size;
	int current_rel = -1;
    bool conflict = false;

	DArray<JournalRecord*> *recs;
	cout << "===========================================" << endl;
	cout << "Checking validation: " << v->validationId << endl;
	cout << "===========================================" << endl;

	// sort queries by column count
	std::sort(v->queries, v->queries + v->queryCount, sorting());

	// for every query of this validation
    for(unsigned i = 0; i < v->queryCount; i++)
    {
		DArray<Bitset*> *bitsets = new DArray<Bitset*>(20);

        QueryPtr q = v->queries[i];

		//=========================
		// empty query ~> conflict
		//=========================
		if (q->columnCount == 0){
			conflict = true; break;}

		//=============================
		// check an einai entos range
		//=============================
		uint64_t max_tid = Journals[q->relationId]->getLastTID();

		if (v->from > max_tid ){	// mou dwse tid start pou einai megalutero tou max || keno query
            // Go to the next query
            // conflict = false;   	// Se periptwsh poy den uparxei allo query
            continue;
        }


		if (current_rel == -1 || current_rel != q->relationId){
			recs = Journals[q->relationId]->getJournalRecords(v->from, v->to);
			current_rel = q->relationId;
		}

		// for every subquery of this query
        for (unsigned w = 0; w < q->columnCount; w++)
        {
            ColumnPtr c = q->columns[w];

			// check if bitset is calculated
            Bitset* bitset = Journals[q->relationId]->val_htable.getbdata(c->key);

            if(bitset == NULL)	// not calculated case
            {
				bitset_size = recs->size();

				// calculate it
                bitset = Journals[q->relationId]->val_htable.UpdateValData(c->key, checkColumn(c, recs), bitset_size);
            }
			// else{
			// 	cout << "exei ypologistei" << endl;
			// }
			if (conflict == false)
				bitsets->push_back(bitset);	//krataw ta bitsets mesa sto query gia na kanw to logical AND
        }
		if (conflict == false){

			int size = bitsets->size();

			// pairnw to prwto bitset gia na kanw logical AND me ta upoloipa

			Bitset* bit = bitsets->get(0);	// pernw to prwto bitset
			int bit_size = bit->getSize();	// pernw to megethos tou
			char *and_result = (char*)malloc(bit_size/8+1);

			memcpy(and_result, bit->getBitsetArray(), sizeof(bit->getSize()));
			// strcpy(and_result, bit->getBitsetArray());

			for (int i = 1; i < size; i ++){
				// printBitset(*bitsets->get(i));

				for (int j = 0; j < bit_size/8+1; j++)
					// TODO: EDW THELW TO RANGE TOU BITSET GIA NA KANW TA AND &
					and_result[j] = and_result[j] & (bitsets->get(i)->getBitsetArray())[j];
			}


			for (int k = 0; k < bit_size/8+1; k++)
			{
				if (and_result[k] != 0){	// an to apotelesma tou and exei kapoion aso mesa exw conf

					// cout << "EXW CONFLICT " << endl;
					conflict = true;
					break;
				}
			}
		}
		delete bitsets;
    }
	if (v->validationId == 20)
		exit(0);
	// cout << "CONFLICT: " << conflict << endl;
	return conflict;
}
//=======================================================================
DArray<bool>* checkColumn(ColumnPtr c, DArray<JournalRecord*> * records)
{
    // DArray<JournalRecord*> Journals[relId]->getJournalRecords()
    DArray<bool>* bitset = new DArray<bool>(20);
    bool result;
    unsigned size = records->size();
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
        bitset->push_back(result);
    }
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
