#include <iostream>
#include "myVector.cpp"

using namespace std;

int main()
{
    myvec vectorA;
    double d = 2.0;
    vectorA.push_back(d);
    cout << vectorA.get(0) << endl;
}
