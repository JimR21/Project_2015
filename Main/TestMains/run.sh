#!/bin/bash

option=0

echo "Choose a Test"
echo

echo "Option[1]: JournalRecord Test Main"
echo "Option[2]: Journal Test Main"
echo "Option[3]: HashTable Test"
echo "Option[4]: DArray Test"

read option

if [ $option == 1 ]; then
	g++ JournalRecordTest/JournalRecordTestMain.cpp JournalRecordTest/JournalRecord.cpp
elif [ $option == 2 ]; then
    g++ JournalTest/JournalTestMain.cpp JournalTest/JournalRecord.cpp Journal.cpp
elif [ $option == 3 ]; then
	g++ -std=c++0x HashTableTest/Bucket.cpp HashTableTest/BucketData.cpp HashTableTest/HashTable.cpp HashTableTest/HashTestMain.cpp
elif [ $option == 4 ]; then
    g++ -std=c++0x DArrayTest/DArrayTest.cpp DArrayTest/JournalRecord.cpp
fi

./a.out
