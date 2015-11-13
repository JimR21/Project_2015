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
    HashTable table;

    auto start = get_time::now(); //use auto keyword to minimize typing strokes :)

    srand(time(0));
    int r;
    for (unsigned i = 0; i < 150000; i++)
    {
        r = rand() % 1000;
        table.insert(r, 6);
        table.get(r);
        cout << "Random = " << r << " found!" << endl;
    }

    cout << "HashTable size = " << table.getsize() << endl;

    auto end = get_time::now();
    auto diff = end - start;
    cout<<"Elapsed time is :  "<< chrono::duration_cast<ns>(diff).count()<<" ms "<<endl;


    return 0;
}
