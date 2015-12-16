#include <iostream>
#include <string>
#include <math.h>

using namespace std;

enum { Equal, NotEqual, Less, LessOrEqual, Greater, GreaterOrEqual };

string stringBuilder(int, int, int, int, int);


int hashFunction(const string& key)
{
    unsigned int h=0;
    const int len=key.length()/2+1;
    for (int i=0; i<len;){
        h=h*i+key[i];
        i+=2;  
    }
    return h%33;
}

int main(){

    int op = 0;
    int column = 1;
    
    string key;
    key = stringBuilder(1,5,2,4,2);
    string key3 = key; cout << key3 + " xax" << endl;
    cout << key << endl;
    cout << hashFunction(key) << endl;
    if (key.compare(key3) == 0) cout << "neti" << endl; 
    string key2;
    key2 = stringBuilder(1,10,6,0,2);
    cout << key2 << endl;
    cout << hashFunction(key2) << endl;
    
    return 0;
}

string stringBuilder(int start, int end, int col, int op, int value){
    string key;
    
    key += to_string(start) + "-" + to_string(end) + " c" + to_string(col);
    
    switch(op){
        case Equal: key += "="; break;
        case NotEqual: key += "!="; break;
        case Less: key += "<"; break;
        case LessOrEqual: key += "<="; break;
        case Greater: key += ">"; break;
        case GreaterOrEqual: key += ">="; break;
    }
    
    key += to_string(value);
    
    return key;
}
