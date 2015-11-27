/* 	-- Test Main gia to JournalRecord --

	Epiloges gia dimiourgia JournalRecords, print kai 'katastrofi' tous

	O xrhsths afou epileksei tin 1i epilogi dinei posa columns tha exei kathe record kai posa records thelei na ftiaksei.
	Sthn sunexeia ta records ftiaxnode me tuxaio Transaction id kai tuxaies columns times to kathena kai apothikevontai
	se enan pinaka (Records).

	Xrhsimopoiountai oi sunartiseis: JournalRecord, ~JournalRecord, addValue. Oi ypoloipes einai polu aples kai den
	parousiazontai.
*/


#include "JournalRecord.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

#define OPTION1 1
#define OPTION2 2
#define OPTION3 -10

using namespace std;

int main(){

	int option, columns, total_records;
	uint64_t* values;
	JournalRecord* record;
	DArray<JournalRecord*> *Records;

	srand(time(0));

	while(1){

		cout << endl;
		cout << "==========================================================" << endl;
		cout << "Options: " << endl << endl;
		cout << "Option[1]: Create JournalRecords - USE THIS ONCE" << endl;
		cout << "Option[2]: Print JournalRecords" << endl;
		cout << "Option[3]: -10 to delete all JournalRecords and EXIT" << endl;
		cout << "==========================================================" << endl;
		cin >> option;

		if (option == OPTION1){

			cout << "Give # of columns for the records: ";
			cin >> columns;
			cout << "Give # of records to create: ";
			cin >> total_records;

			values = (uint64_t*)malloc((columns * total_records) * sizeof(uint64_t));
			Records = new DArray<JournalRecord*>();

			// array with random record values for total_records records
			for (int i = 0; i < columns * total_records; i++){
				uint64_t val_to_add = rand() % 10001;
				values[i] = val_to_add;
			}

			for (int i = 0; i < columns * total_records; i++){
				if(i % columns == 0) {		// start of group
					uint64_t random_tid = rand() % 100;
					record = new JournalRecord(random_tid, INSERT);
				}
				record->addValue(values[i]);  // add value to record

				if((i + 1) % columns == 0) 	  // end of group
					Records->push_back(record);
			}
		}
		else if (option == OPTION2){
			int index;
			cout << "Select Record to print (0-" << total_records - 1 << ") [-1 to print ALL]" << endl;
			cin >> index;

			if (index == -1)
				for (int i = 0 ; i < total_records; i++)
					Records->get(i)->printRecord();
			else
				Records->get(index)->printRecord();
		}
		else if (option == OPTION3){
			for (int i = 0 ; i < total_records; i++)
				delete Records->get(i);

			delete Records;
			free(values);
			return 1;
		}
	}
	return 0;
}
