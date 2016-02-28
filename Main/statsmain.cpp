
// Statistics main

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <iostream>
#include "Journal.hpp"

using namespace std;

ofstream myfile ("out.bin", ios::out);

class Stats
{
protected:
    unsigned counter;
    unsigned max;
    unsigned min;
    double mo;    // mesos oros

public:
    Stats():counter(0), max(0), min(0), mo(0){};

    void update()
    {
        counter ++;
    };
};

class SchemaStats : public Stats
{
protected:


public:

    void update(unsigned col)
    {
        counter ++;

        if(max == 0 && min == 0)
        {
            max = col;
            min = col;
        }
        else if(col > max)
            max = col;
        else if(col < min)
            min = col;
    };

    void printStats()
    {
        cout << "================== Schema Stats ==================" << endl;
        cout << "Total : " << counter << endl;
        cout << "Max Columns : " << max << endl;
        cout << "Min Columns : " << min << endl;
        cout << "==================================================" << endl;
        cout << endl;
    };
};


class TransStats : public Stats
{
protected:
    unsigned inserts;
    unsigned deletes;
    unsigned insertRows;
    unsigned deleteRows;
    unsigned maxinsert;
    unsigned mininsert;
    unsigned maxdelete;
    unsigned mindelete;


public:
    TransStats(): inserts(0), deletes(0), insertRows(0), deleteRows(0)
    {
        counter = 0;
        min = 0;
        max = 0;
        mo = 0;
    };

    void update(unsigned ins, unsigned del)
    {
        inserts += ins;
        deletes += del;
        counter ++;
    };

    void updateInsertRows(unsigned r)
    {
        insertRows += r;

        if(maxinsert == 0 && mininsert == 0)
        {
            maxinsert = r;
            mininsert = r;
        }
        else if(r > maxinsert)
            maxinsert = r;
        else if(r < mininsert)
            mininsert = r;
    };

    void updateDeleteRows(unsigned r)
    {
        deleteRows += r;

        if(maxdelete == 0 && mindelete == 0)
        {
            maxdelete = r;
            mindelete = r;
        }
        else if(r > maxdelete)
            maxdelete = r;
        else if(r < mindelete)
            mindelete = r;
    };

    void printStats()
    {
        cout << "================ Transaction Stats ===============" << endl;
        cout << "Total : " << counter << endl;
        cout << "Inserts : " << inserts << endl;
        cout << "Deletes : " << deletes << endl;
        cout << "Insert Rows: " << insertRows << endl;
        cout << "Delete Rows : " << deleteRows << endl;
        cout << "Average Insert Rows : " << (double)insertRows / inserts << endl;
        cout << "Average Delete Rows : " << (double)deleteRows / deletes << endl;
        cout << "Max Insert Rows : " << maxinsert << endl;
        cout << "Min Insert Rows : " << mininsert << endl;
        cout << "Max Delete Rows : " << maxdelete << endl;
        cout << "Min Delete Rows : " << mindelete << endl;
        cout << "==================================================" << endl;
        cout << endl;
    };
};

class ValStats : public Stats
{
protected:
    unsigned queries;
    unsigned subqueries;
    unsigned minquery;
    unsigned maxquery;
    unsigned minsub;
    unsigned maxsub;

public:
    ValStats(): queries(0), subqueries(0), minquery(0), maxquery(0), minsub(0), maxsub(0)
    {
        counter = 0;
        min = 0;
        max = 0;
        mo = 0;
    };

    void update(unsigned q)
    {
        counter ++;
        queries += q;

        if(maxquery == 0 && minquery == 0)
        {
            maxquery = q;
            minquery = q;
        }
        else if(q > maxquery)
            maxquery = q;
        else if(q < minquery)
            minquery = q;
    };

    void updateSub(unsigned s)
    {
        subqueries += s;

        if(maxsub == 0 && minsub == 0)
        {
            maxsub = s;
            minsub = s;
        }
        else if(s > maxsub)
            maxsub = s;
        else if(s < minsub)
            minsub = s;
    };

    unsigned getSubs()
    {
        return subqueries;
    }


    void printStats()
    {
        cout << "=============== Validation Stats =================" << endl;
        cout << "Total : " << counter << endl;
        cout << "Queries : " << queries << endl;
        cout << "Max Queries in 1 Validation : " << maxquery << endl;
        cout << "Min Queries in 1 Validation : " << minquery << endl;
        cout << "Average Queries in 1 Validation: " << (double)queries/counter << endl;
        cout << "Subqueries : " << subqueries << endl;
        cout << "Max Subqueries in 1 Query : " << maxsub << endl;
        cout << "Min Subqueries in 1 Query : " << minsub << endl;
        cout << "Average Subqueries in 1 Query: " << (double)subqueries/queries << endl;
        cout << "==================================================" << endl;
        cout << endl;
    };
};

class FlStats : public Stats
{
public:
    void printStats()
    {
        cout << "================== Flush Stats ===================" << endl;
        cout << "Total : " << counter << endl;
        cout << "==================================================" << endl;
        cout << endl;
    }
};

class ForStats : public Stats
{
public:
    void printStats()
    {
        cout << "================== Forget Stats ==================" << endl;
        cout << "Total : " << counter << endl;
        cout << "==================================================" << endl;
        cout << endl;
    }
};

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
//================================================================================================
class SpecialStats
{
protected:
    unsigned eqc0;  // c0 ==
    unsigned eqc1;
    unsigned eqc2;
    unsigned eqc3;
    unsigned eq;    // Genika ==
    unsigned gen;   // >, <, !=

public:
    SpecialStats() : eqc0(0), eqc1(0), eqc2(0), eqc3(0), eq(0), gen(0){};

    void update(const Query* q)
    {
        for (unsigned w = 0; w < q->columnCount; w++)
        {
            if (q->columns[w].op == Query::Column::Equal && q->columns[w].column == 0)
                eqc0++;
            else if(q->columns[w].op == Query::Column::Equal && q->columns[w].column == 1)
                eqc1++;
            else if(q->columns[w].op == Query::Column::Equal && q->columns[w].column == 2)
            eqc2++;
            else if(q->columns[w].op == Query::Column::Equal && q->columns[w].column == 3)
            eqc3++;
            else if(q->columns[w].op == Query::Column::Equal)
                eq++;
            else
                gen++;
		}
    }

    void printStats(unsigned sub)
    {
        cout << "================ *SPECIAL STATS* =================" << endl;
        cout << "------------- Validation Predicates --------------" << endl;
        cout << "|c0 = a| ...................... " << eqc0 <<" (" << (double)eqc0/sub * 100 << "\%)" << endl;
        cout << "|c1 = a| ...................... " << eqc1 <<" (" << (double)eqc1/sub * 100 << "\%)" << endl;
        cout << "|c2 = a| ...................... " << eqc2 <<" (" << (double)eqc2/sub * 100 << "\%)" << endl;
        cout << "|c3 = a| ...................... " << eqc3 <<" (" << (double)eqc3/sub * 100 << "\%)" << endl;
        cout << "|cX = a| (X != 0) ............. " << eq <<" (" << (double)eq/sub * 100 << "\%)" << endl;
        cout << "|cX > | < | != a| ............. " << gen <<" (" << (double)gen/sub * 100 << "\%)" << endl;
        cout << "==================================================" << endl;
        cout << endl;
    }
};
//================================================================================================

SchemaStats ss;
TransStats ts;
ValStats vs;
FlStats fls;
ForStats frs;
SpecialStats specs;

Journal** Journals;


static uint32_t* schema = NULL;
static void processDefineSchema(DefineSchema *s){
    unsigned i;
    // printf("DefineSchema %d |", s->relationCount);
    if ( schema == NULL) free(schema);
    schema = (uint32_t*)malloc(sizeof(uint32_t)*s->relationCount);

    for(i = 0; i < s->relationCount; i++) {
    // printf(" %d ",s->columnCounts[i]);
    schema[i] = s->columnCounts[i];

        ss.update(s->columnCounts[i]);
    }
    // printf("\n");
}

static void processTransaction(Transaction *t){
    unsigned i;
    const char* reader = t->operations;
    // printf("Transaction %lu (%u, %u) |", t->transactionId, t->deleteCount, t->insertCount);

    ts.update(t->insertCount, t->deleteCount);

    for(i=0; i < t->deleteCount; i++) {
    const TransactionOperationDelete* o = (TransactionOperationDelete*)reader;

    ts.updateDeleteRows(o->rowCount);

    // printf("opdel rid %u #rows %u ", o->relationId, o->rowCount);
    reader+=sizeof(TransactionOperationDelete)+(sizeof(uint64_t)*o->rowCount);
    }
    // printf(" \t| ");
    for(i=0; i < t->insertCount; i++) {
    const TransactionOperationInsert* o = (TransactionOperationInsert*)reader;

    ts.updateInsertRows(o->rowCount);

    // printf("opins rid %u #rows %u |", o->relationId, o->rowCount);
    reader+=sizeof(TransactionOperationInsert)+(sizeof(uint64_t)*o->rowCount*schema[o->relationId]);
    }
    // printf("\n");

}
static void processValidationQueries(ValidationQueries *v){
    // printf("ValidationQueries %lu [%lu, %lu] %u\n", v->validationId, v->from, v->to, v->queryCount);

    vs.update(v->queryCount);

    const char* reader = v->queries;

    for (unsigned i = 0; i != v->queryCount; i++)
    {
		const Query* q = (Query*)reader;

        vs.updateSub(q->columnCount);

        specs.update(q);

        reader += sizeof(Query)+(sizeof(Query::Column)*q->columnCount);
     }


}
static void processFlush(Flush *fl){
    // printf("Flush %lu\n", fl->validationId);

    fls.update();

}
static void processForget(Forget *fo){
    // printf("Forget %lu\n", fo->transactionId);

    frs.update();

}


int main(int argc, char **argv) {

  MessageHead head;
  void *body = NULL;
  uint32_t len;

    while(1){
      // Retrieve the message head
      if (read(0, &head, sizeof(head)) <= 0) { return -1; } // crude error handling, should never happen
    //   printf("HEAD LEN %u \t| HEAD TYPE %u \t| DESC ", head.messageLen, head.type);

      // Retrieve the message body
      if (body != NULL) free(body);
      if (head.messageLen > 0 ){
      body = malloc(head.messageLen*sizeof(char));
      if (read(0, body, head.messageLen) <= 0) { printf("err");return -1; } // crude error handling, should never happen
      len-=(sizeof(head) + head.messageLen);
      }

      // And interpret it
      switch (head.type) {
        case MessageHead::Done: printf("\n\n");

        ss.printStats();
        ts.printStats();
        vs.printStats();
        fls.printStats();
        frs.printStats();
        specs.printStats(vs.getSubs());

        return 0;
        case MessageHead::DefineSchema: processDefineSchema((DefineSchema *)body); break;
        case MessageHead::Transaction: processTransaction((Transaction *)body); break;
        case MessageHead::ValidationQueries: processValidationQueries((ValidationQueries *)body); break;
        case MessageHead::Flush: processFlush((Flush *)body); break;
        case MessageHead::Forget: processForget((Forget *)body); break;
        default:
        return -1; // crude error handling, should never happen
      }
    }

  return 0;
}
