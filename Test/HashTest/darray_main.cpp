#include <iostream>
#include "Bucket.hpp"

using namespace std;

int main()
{
    DArray<Bucket*> t1;

    for(int i = 0; i < 100000; i++)
        t1.push_back(new Bucket());

        cout << t1.get(5) << endl;
        t1.popAt(5);
        cout << t1.get(5) << endl;

        cout << t1.get(t1.size()-1) << endl;
        t1.popLast();
        cout << t1.get(t1.size()-1) << endl;
}
