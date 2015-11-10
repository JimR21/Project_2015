/*
 * Inspired from SIGMOD Programming Contest 2015.
 * http://db.in.tum.de/sigmod15contest/task.html
 * Simple requests parsing and reporting.
**/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>
//---------------------------------------------------------------------------

struct MessageHead {
   /// Message types
   enum Type : uint32_t { Done, DefineSchema, Transaction, ValidationQueries, Flush, Forget };
   /// Total message length, excluding this head
   uint32_t messageLen;
   /// The message type
   Type type;
};
//---------------------------------------------------------------------------
struct DefineSchema {
   /// Number of relations
   uint32_t relationCount;
   /// Column counts per relation, one count per relation. The first column is always the primary key
   uint32_t columnCounts[];
};
//---------------------------------------------------------------------------
struct Transaction {
   /// The transaction id. Monotonic increasing
   uint64_t transactionId;
   /// The operation counts
   uint32_t deleteCount,insertCount;
   /// A sequence of transaction operations. Deletes first, total deleteCount+insertCount operations
   char operations[];
};
//---------------------------------------------------------------------------
struct TransactionOperationDelete {
   /// The affected relation
   uint32_t relationId;
   /// The row count
   uint32_t rowCount;
   /// The deleted values, rowCount primary keyss
   uint64_t keys[];
};
//---------------------------------------------------------------------------
struct TransactionOperationInsert {
   /// The affected relation
   uint32_t relationId;
   /// The row count
   uint32_t rowCount;
   /// The inserted values, rowCount*relation[relationId].columnCount values
   uint64_t values[];
};
//---------------------------------------------------------------------------
struct ValidationQueries {
   /// The validation id. Monotonic increasing
   uint64_t validationId;
   /// The transaction range
   uint64_t from,to;
   /// The query count
   uint32_t queryCount;
   /// The queries
   char queries[];
};
//---------------------------------------------------------------------------
struct Query {
   /// A column description
   struct Column {
      /// Support operations
      enum Op : uint32_t { Equal, NotEqual, Less, LessOrEqual, Greater, GreaterOrEqual };
      /// The column id
      uint32_t column;
      /// The operations
      Op op;
      /// The constant
      uint64_t value;
   };

   /// The relation
   uint32_t relationId;
   /// The number of bound columns
   uint32_t columnCount;
   /// The bindings
   Column columns[];
};
//---------------------------------------------------------------------------
struct Flush {
   /// All validations to this id (including) must be answered
   uint64_t validationId;
};
//---------------------------------------------------------------------------
struct Forget {
   /// Transactions older than that (including) will not be tested for
   uint64_t transactionId;
};

static uint32_t* schema = NULL;
static void processDefineSchema(DefineSchema *s){
  int i;
  printf("DefineSchema %d |", s->relationCount);
  if ( schema == NULL) free(schema);
  schema = (uint32_t *)malloc(sizeof(uint32_t)*s->relationCount);
  
  for(i = 0; i < s->relationCount; i++) {
    printf(" %d ",s->columnCounts[i]);
    schema[i] = s->columnCounts[i];
	
	
  }
  printf("\n");
}

static void processTransaction(Transaction *t){
  int i;
  const char* reader = t->operations;
  printf("Transaction %llu  (%u, %u) |", t->transactionId, t->deleteCount, t->insertCount);
  for(i=0; i < t->deleteCount; i++) {
    const TransactionOperationDelete* o = (TransactionOperationDelete*)reader;
    printf("opdel rid %u #rows %u ", o->relationId, o->rowCount);
    reader+=sizeof(TransactionOperationDelete)+(sizeof(uint64_t)*o->rowCount);
  }
  printf(" \t| ");
  for(i=0; i < t->insertCount; i++) {
    const TransactionOperationInsert* o = (TransactionOperationInsert*)reader;
	
    printf("opins rid %u #rows %u | ", o->relationId, o->rowCount);
	int j;
	
	printf("(");
	for(j=0;j< o->rowCount * schema[o->relationId] ;j++) {
		printf("%llu ", o->values[j]);
		if((j+1)%schema[o->relationId]==0) {
			printf(")");
			if(j+1 < o->rowCount * schema[o->relationId] ) printf("(");
		}
		
	}
    reader+=sizeof(TransactionOperationInsert)+(sizeof(uint64_t)*o->rowCount*schema[o->relationId]);
  }
  printf("\n");
  
}
static void processValidationQueries(ValidationQueries *v){
  printf("ValidationQueries %llu [%llu, %llu] %u\n", v->validationId, v->from, v->to, v->queryCount);
}
static void processFlush(Flush *fl){
 printf("Flush %llu\n", fl->validationId);

}
static void processForget(Forget *fo){
  printf("Forget %llu\n", fo->transactionId);

}


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
         case MessageHead::Done: printf("\n");return 0;
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
