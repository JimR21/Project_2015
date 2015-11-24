#include "HashTable.hpp"
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <chrono>


using namespace std;
using ns = chrono::nanoseconds;
using get_time = chrono::steady_clock ;

int main(int argc, char const *argv[])
{
    HashTable* table = new HashTable();

    auto start = get_time::now(); //use auto keyword to minimize typing strokes :)

    // srand(time(0));
    // unsigned key, tid, offset, ret;
    // for (unsigned i = 0; i < 100000; i++)
    // {
    //     key = rand() % 1000000;
    //     table->insert(5, 5, i);
    //
    // }
    //
    // ret = table->getLastJournalInsert(5);
    // cout << " Random = " << 5 << " found!" << "Offset = " << ret << endl;
    //
    // cout << "HashTable size = " << table->getsize() << endl;


    table->insert(4,4,1);
    table->insert(129,5,1);
    table->deleteKey(129);

    auto end = get_time::now();
    auto diff = end - start;
    cout<<"Elapsed time is :  "<< chrono::duration_cast<ns>(diff).count()<<" ms "<<endl;

    delete table;

    return 0;
}
