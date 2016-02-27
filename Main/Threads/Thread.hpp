#ifndef THREAD_HPP
#define THREAD_HPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ThreadInterface.hpp"
#include "../ValidationNode.hpp"

#if VAL_THREADS == 1

using namespace std;

extern unsigned jobs;			// declared at main.cpp
extern Journal** Journals;	// declared at main.cpp
extern pthread_mutex_t counter_mutex;
extern pthread_cond_t  counter_cv;

class Thread : public ThreadInterface
{
	DArray<ValidationNode*>& validationList;

	void valOptimize();
public:

	Thread(DArray<ValidationNode*>& validations) : validationList(validations) {}

	// =============================================================================================
    void *run() {
		bool conflict;
		// Remove 1 item at a time and process it. Blocks if no items are available to process.
        for (int i = 0;; i++) {

			ValidationNode* valNode = (ValidationNode*)validationList.safe_fake_popGetLast();
			ValClass* val = valNode->getValidation();

			if (valNode->getResult() == false){
				conflict = valOptimize(val);	// calculate validation
				valNode->setResult(conflict);	// set the result in the validation
			}
			pthread_mutex_lock(&counter_mutex);	// lock counter
				jobs--;
				if (jobs == 0){
					pthread_cond_signal(&counter_cv);
				}
		    pthread_mutex_unlock(&counter_mutex);	// unlock counter

        }
        return NULL;
    }
	// =============================================================================================
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
            	continue;			// no need to check the next queries for this validation
	        }

	        if (q->columnCount == 0)    // an einai keno kai mesa sta oria tote conflict
				continue;

	        conflict = q->validate(*Journals[q->relationId], v->from, v->to);

			if(conflict == true)
				break;
	    }

	    return conflict;
	}
};

#endif
#endif
