#/bin/bash

g++ -c -std=c++0x -w main.cpp Journal.cpp JournalRecord.cpp Gen_HashTable.cpp  Key_HashTable.cpp  Val_HashTable.cpp Tid_HashTable.cpp  Bucket.cpp BucketData.cpp BucketVal.cpp Val_bdata.cpp ValidationIndex.cpp valClass.cpp queryClass.cpp columnClass.cpp bitset.cpp Val_listbucket.cpp ValidationNode.cpp Threads/ThreadInterface.cpp

g++ -pthread main.o Journal.o JournalRecord.o Gen_HashTable.o Key_HashTable.o Val_HashTable.o Tid_HashTable.o Bucket.o BucketData.o BucketVal.o Val_bdata.o ValidationIndex.o valClass.o queryClass.o columnClass.o bitset.o Val_listbucket.o ValidationNode.o ThreadInterface.o -o test.out
