#include "HashTable.hpp"
#include <cstdlib>
#include <iostream>
#include <ctime>


using namespace std;

int main(int argc, char const *argv[])
{
    HashTable table;
    srand(time(0));
    int r;
    unsigned p;
    for (unsigned i = 0; i < 50000; i++)
    {
        r = rand() % 1000000;
        table.insert(r, 6);
        table.get(r);
        cout << "Random = " << r << " found!" << endl;
    }

    cout << "HashTable size = " << table.getsize() << endl;


    return 0;
}
