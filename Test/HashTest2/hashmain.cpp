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
    unsigned int r,r2,r3;
    // for (unsigned i = 0; i < 100; i++)
    // {
        // r = rand() % 1000000;	//column 0 values
		r = 1;
		// cout << "Key: " << r << endl;
		table.insert(r, 6);
		r2 = 3;
        table.insert(r2, 6);
		r3 = 5;
		table.insert(r3, 6);
		table.get(r);
		table.get(r2);
        table.get(r3);
		// cout << i << " Random = " << r << " found!" << endl;
        // cout << " key = " << d << " found!" << endl;
    // }

    cout << "HashTable size = " << table.getsize() << endl;

    auto end = get_time::now();
    auto diff = end - start;
    cout<<"Elapsed time is :  "<< chrono::duration_cast<ns>(diff).count()<<" ms "<<endl;


    return 0;
}
