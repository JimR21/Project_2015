#ifndef THREAD_HPP
#define THREAD_HPP


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ThreadInterface.hpp"
#include "../ValidationNode.hpp"

class Thread : public ThreadInterface
{
	DArray<ValidationNode*>& validationList;

public:

	Thread(DArray<ValidationNode*>& validations) : validationList(validations) {}

    void *run() {
		// Remove 1 item at a time and process it. Blocks if no items are available to process.
        for (int i = 0;; i++) {
            printf("thread %lu, loop %d - waiting for item...\n", (long unsigned int)self(), i);

			ValidationNode* valNode = (ValidationNode*)validationList.safe_popGetLast();
			ValClass* val = valNode->getValidation();

            printf("thread %lu, loop %d - got one item\n", (long unsigned int)self(), i);
			printf("thread %lu, loop %d, calculating validation: %lu\n", (long unsigned int)self(), i, val->validationId);
			// delete item;
        }
        return NULL;
    }
};

#endif
