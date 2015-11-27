#include "Journal.h"
#include <cstdlib>
#include <ctime>

#define OPTION1 1
#define OPTION2 2
#define OPTION3 3
#define OPTION4 4

using namespace std;

int main(){

	int tid = 0;
	int option;
	Journal* myJournal = new Journal(0);

	while (1){
		cout << endl;
		cout << "=============================================================" << endl;
		cout << "Options: " << endl << endl;
		cout << "Option[1]: Create a random JournalRecord and add it to Journal" << endl;
		cout << "Option[2]: Search for a transactionId in Journal" << endl;
		cout << "Option[3]: Print Journal" << endl;
		cout << "Option[4]: Delete Journal and EXIT" << endl;
		cout << "=============================================================" << endl;
		cin >> option;

		if (option == OPTION1){
			JournalRecord* record = new JournalRecord(tid, INSERT);
			tid++;

			uint64_t c0 = rand() % 10;
			uint64_t c1 = rand() % 10;
			uint64_t c2 = rand() % 10;
			record->addValue(c0);
			record->addValue(c1);
			record->addValue(c2);

			myJournal->insertJournalRecord(record);
		}
		else if (option == OPTION2){
			int tsearch;
			cout << "Type transaction id to search: ";
			cin >> tsearch;

			int result = myJournal->searchRecord(tsearch);

			if (result == -1)	cout << "Tid not found" << endl;
			else	cout << "Key Found at index: " << result << endl;
		}
		else if (option == OPTION3)
			myJournal->printJournal();
		else if (option == OPTION4){
			myJournal->destroyJournal();
			delete myJournal;
			return 1;
		}
	}
	return 0;
}
