#include <iostream>
#include <stdint.h>
#include "DArray2.hpp"
#include "Bucket.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
    DArray<Bucket> barray;
    Bucket b;
    b.insert(5,6);
    barray.push_back(b);
    barray.push_back(b);
    Bucket c = barray.get(1);

    cout << "Bucket c key = " << c.getkey() << endl;
    return 0;
}
