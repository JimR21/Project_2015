#include "HashTable.hpp"
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <chrono>

#define MAXKEYS 100000
#define MAX 1000000

using namespace std;
using ns = chrono::milliseconds;
using get_time = chrono::steady_clock ;

int main(int argc, char const *argv[])
{
    HashTable table;

    int keys[MAXKEYS]; //array to store the random numbers in

    srand(1); //always seed your RNG before using it

    //generate random numbers:
    for (int i=0;i<MAXKEYS;i++)
    {
        bool check; //variable to check or number is already used
        int n; //variable to store the number in
        do
        {
        n=rand()%MAX;
        //check or number is already used:
        check=true;
        for (int j=0;j<i;j++)
            if (n == keys[j]) //if number is already used
            {
                check=false; //set check to false
                break; //no need to check the other elements of keys[]
            }
        } while (!check); //loop until new, unique number is found
        keys[i]=n; //store the generated number in the array
    }

    auto start = get_time::now(); //use auto keyword to minimize typing strokes :)

    unsigned i, ret;
    for (i = 0; i < MAXKEYS; i++)
    {
        table.insert(keys[i], 5, i);
    }

    // for(i = 0; i < MAXKEYS; i++)
    // {
    //     ret = table->getLastJournalInsert(keys[i]);
    //     //cout << " Key = " << keys[i] << " found!" << "Offset = " << ret << endl;
    // }

    // for(i = 0; i < MAXKEYS; i++)
    // {
    //     table->deleteKey(keys[i]);
    //     //cout << " Key = " << keys[i] << " deleted!" << "offset = " << i << endl;;
    // }

    cout << "HashTable size = " << table.getsize() << endl;

    // table->insert(516, 5, 6);
    //
    // table->insert(4,4,1);
    //
    // table->insert(132,5,1);
    //
    // table->insert(260, 5, 1);
    //
    //
    // if(table->getLastJournalInsert(132) != -1)
    // {
    //     cout << "Key = " << 132 << " found!" << endl;
    // }
    // if(table->getLastJournalInsert(4) != -1)
    // {
    //     cout << "Key = " << 4 << " found!" << endl;
    // }
    // if(table->getLastJournalInsert(260) != -1)
    // {
    //     cout << "Key = " << 260 << " found!" << endl;
    // }
    //
    //
    // table->deleteKey(4);
    // if(table->getLastJournalInsert(132) != -1)
    // {
    //     cout << "Key = " << 132 << " found!" << endl;
    // }
    // if(table->getLastJournalInsert(4) != -1)
    // {
    //     cout << "Key = " << 4 << " found!" << endl;
    // }
    // if(table->getLastJournalInsert(260) != -1)
    // {
    //     cout << "Key = " << 260 << " found!" << endl;
    // }
    //
    // table->deleteKey(132);
    // if(table->getLastJournalInsert(132) != -1)
    // {
    //     cout << "Key = " << 132 << " found!" << endl;
    // }
    // if(table->getLastJournalInsert(4) != -1)
    // {
    //     cout << "Key = " << 4 << " found!" << endl;
    // }
    // if(table->getLastJournalInsert(260) != -1)
    // {
    //     cout << "Key = " << 260 << " found!" << endl;
    // }
    //
    // table->deleteKey(260);
    // if(table->getLastJournalInsert(132) != -1)
    // {
    //     cout << "Key = " << 132 << " found!" << endl;
    // }
    // if(table->getLastJournalInsert(4) != -1)
    // {
    //     cout << "Key = " << 4 << " found!" << endl;
    // }
    // if(table->getLastJournalInsert(260) != -1)
    // {
    //     cout << "Key = " << 260 << " found!" << endl;
    // }
    //
    // table->deleteKey(516);

    auto end = get_time::now();
    auto diff = end - start;
    cout<<"Elapsed time is :  "<< chrono::duration_cast<ns>(diff).count()<<" ms "<<endl;

    return 0;
}
