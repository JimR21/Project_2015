#ifndef KEY_HASHTABLE_HPP
#define KEY_HASHTABLE_HPP
#include "Gen_HashTable.hpp"

#define HASHTABLE_SIZE 128
#define STARTING_GLOBAL_DEAPTH 7

class Bucket;

class Key_HashTable : public Gen_HashTable
{
    friend class Bucket;
public:

    void insert(unsigned key, unsigned int tid, unsigned int offset);

	DArray<uint64_t>* getHashRecord(unsigned key, uint64_t start_tid, uint64_t end_tid);

	DArray<uint64_t>* getHashRecords(unsigned key);

	bool existCheck(unsigned key,uint64_t, uint64_t);

    // Returns offset for Journal
	int getLastJournalInsert(unsigned key);
};


#endif