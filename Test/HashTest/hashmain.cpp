#include "HashTable.hpp"
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <chrono>

#define MAXKEYS 100

using namespace std;
using ns = chrono::nanoseconds;
using get_time = chrono::steady_clock ;

int main(int argc, char const *argv[])
{
    HashTable* table = new HashTable();

    auto start = get_time::now(); //use auto keyword to minimize typing strokes :)

    // srand(time(0));
    // unsigned i, ret, keys[MAXKEYS];
    // for (i = 0; i < MAXKEYS; i++)
    // {
    //     keys[i] = rand() % 1000000;
    //     table->insert(keys[i], 5, i);
    // }
    //
    // for(i = 0; i < MAXKEYS; i++)
    // {
    //     ret = table->getLastJournalInsert(keys[i]);
    //     cout << " Key = " << keys[i] << " found!" << "Offset = " << ret << endl;
    // }
    //
    // for(i = 0; i < MAXKEYS; i++)
    // {
    //     table->deleteKey(keys[i]);
    //     cout << " Key = " << keys[i] << " deleted!" << endl;;
    // }
    //
    // cout << "HashTable size = " << table->getsize() << endl;


    table->insert(4,4,1);

    table->insert(132,5,1);

    table->insert(260, 5, 1);

    if(table->getLastJournalInsert(132) != -1)
    {
        cout << "Key = " << 132 << " found!" << endl;
    }
    if(table->getLastJournalInsert(4) != -1)
    {
        cout << "Key = " << 4 << " found!" << endl;
    }
    if(table->getLastJournalInsert(260) != -1)
    {
        cout << "Key = " << 260 << " found!" << endl;
    }


    table->deleteKey(4);
    if(table->getLastJournalInsert(132) != -1)
    {
        cout << "Key = " << 132 << " found!" << endl;
    }
    if(table->getLastJournalInsert(4) != -1)
    {
        cout << "Key = " << 4 << " found!" << endl;
    }
    if(table->getLastJournalInsert(260) != -1)
    {
        cout << "Key = " << 260 << " found!" << endl;
    }

    table->deleteKey(132);
    if(table->getLastJournalInsert(132) != -1)
    {
        cout << "Key = " << 132 << " found!" << endl;
    }
    if(table->getLastJournalInsert(4) != -1)
    {
        cout << "Key = " << 4 << " found!" << endl;
    }
    if(table->getLastJournalInsert(260) != -1)
    {
        cout << "Key = " << 260 << " found!" << endl;
    }

    table->deleteKey(260);
    if(table->getLastJournalInsert(132) != -1)
    {
        cout << "Key = " << 132 << " found!" << endl;
    }
    if(table->getLastJournalInsert(4) != -1)
    {
        cout << "Key = " << 4 << " found!" << endl;
    }
    if(table->getLastJournalInsert(260) != -1)
    {
        cout << "Key = " << 260 << " found!" << endl;
    }

    auto end = get_time::now();
    auto diff = end - start;
    cout<<"Elapsed time is :  "<< chrono::duration_cast<ns>(diff).count()<<" ms "<<endl;

    delete table;

    return 0;
}
