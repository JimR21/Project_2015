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

    srand(time(0));
    unsigned r;
    for (unsigned i = 0; i < 1000000; i++)
    {
        r = rand() % 1000000;
        table->insert(r, i);
        table->get(r);
        //cout << i << " Random = " << r << " found!" << endl;
    }

    cout << "HashTable size = " << table->getsize() << endl;

    auto end = get_time::now();
    auto diff = end - start;
    cout<<"Elapsed time is :  "<< chrono::duration_cast<ns>(diff).count()<<" ms "<<endl;

    delete table;


    return 0;
}
