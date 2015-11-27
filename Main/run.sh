#!/bin/bash

option=0

echo "Choose a Test"
echo

echo "Option[1]: JournalRecord Test Main"
echo "Option[2]: Journal Test Main"

read option

if [ $option == 1 ]; then
	g++ JournalRecordTestMain.cpp JournalRecord.cpp
elif [ $option == 2]; then
    g++ JournalTestMain.cpp JournalRecord.cpp Journal.cpp
fi

./a.out
