#include "DArray.hpp"
#include "JournalRecord.hpp"
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <time.h>

#define OPTION1 1
#define OPTION2 2
#define OPTION3 3

using namespace std;


int main(int argc, char const *argv[]){

	srand(time(NULL));
	int option;

	while(1)
    {
        cout << endl;
        cout << "==========================================================" << endl;
        cout << "Options: " << endl << endl;
        cout << "Option[1]: Create Arrays of different types and print results" << endl;
        cout << "Option[2]: Exit" << endl;
        cout << "==========================================================" << endl;
        cin >> option;

        if(option == OPTION1){
			cout << "Creating <int> array" << endl;
        	DArray<int> int_array;

			cout << "Creating <double> array" << endl;
			DArray<double> double_array;

			cout << "Creating <JournalRecord*> array" << endl;
			DArray<JournalRecord*> object_Array;

			cout << "Adding Values to the arrays..." << endl;
			for (int i = 0; i < 5; i ++){
				int_array.push_back(rand() % 100);
				double_array.push_back(0 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(100-0))));

				JournalRecord* record = new JournalRecord(rand() % 20, 0, 32);
				object_Array.push_back(record);
			}
			cout << "Printing Arrays" << endl;
			cout << "==========================================" << endl;
			cout << "Array of Integers" << endl;
			cout << "==========================================" << endl;
			for (int i = 0; i < 5; i ++)
				cout << "Int array value[" << i << "] = "<< int_array.get(i) << endl;
			cout << "==========================================" << endl;
			cout << "Array of Doubles" << endl;
			cout << "==========================================" << endl;
			for (int i = 0; i < 5; i ++)
				cout << "Double array value[" << i << "] = "<< double_array.get(i) << endl;
			cout << "==========================================" << endl;
			cout << "Array of JournalRecord objects" << endl;
			cout << "==========================================" << endl;
			for (int i = 0; i < 5; i ++)
				cout << "JournalRecords array record[" << i << "], tid = "<< object_Array.get(i)->getTransactionId() << endl;
		}
		else if (option == OPTION2)
			exit(0);
	}
	return 0;
}
