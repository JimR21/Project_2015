/* Main for testing HashTable */

#include "HashTable.hpp"
#include <iostream>
#include <ctime>
#include <chrono>

#define OPTION1 1
#define OPTION2 2
#define OPTION3 3
#define OPTION4 4
#define OPTION5 5
#define OPTION6 6
#define OPTION7 7
#define OPTION8 8

using namespace std;
using ns = chrono::milliseconds;
using get_time = chrono::steady_clock;

int main(int argc, char const *argv[])
{
    HashTable *table;
    int option, keyNumber, max;
    bool deleted = true;
    DArray<int> keys;

    while(1)
    {
        cout << endl;
        cout << "==========================================================" << endl;
        cout << "Options: " << endl << endl;
        cout << "Option[1]: Create HashTable" << endl;
        cout << "Option[2]: Print Keys" << endl;
        cout << "Option[3]: Insert Key" << endl;
        cout << "Option[4]: Delete Key" << endl;
        cout << "Option[5]: Get Last Journal Record Offset of Key" << endl;
        cout << "Option[6]: Get Journal Record Offsets of key in range" << endl;
        cout << "Option[7]: Delete all Keys" << endl;
        cout << "Option[8]: Exit" << endl;
        cout << "==========================================================" << endl;
        cin >> option;

        if(option == OPTION1)
        {
            if(deleted == false)
            {
                cout << "Deleting previous HashTable... ";
                delete table;
                keys.changeSize(0);
                cout << "✔" << endl;
            }

            table = new HashTable();

            cout << "Number of keys to insert (0 = empty): ";
            cin >> keyNumber;
            if (keyNumber > 0)
            {
                cout << "Max value of keys: ";
                cin >> max;


                cout << endl;
                cout << "Generating " << keyNumber << " different random keys with max: " << max << "... ";

                //generate random numbers:
                srand(1);
                for (int i=0;i<keyNumber;i++)
                {
                    bool check; //variable to check or number is already used
                    int n; //variable to store the number in
                    do
                    {
                    n=rand()%max;
                    //check or number is already used:
                    check=true;
                    for (int j=0;j<i;j++)
                        if (n == keys.get(j)) //if number is already used
                        {
                            check=false; //set check to false
                            break; //no need to check the other elements of keys[]
                        }
                    } while (!check); //loop until new, unique number is found
                    keys.push_back(n); //store the generated number in the array
                }
                cout << "✔" << endl;

                auto start = get_time::now();

                cout << "Inserting keys with tid = 0 and incremented offset... ";
                for(int i = 0; i < keyNumber; i++)
                    table->insert(keys.get(i), 0, i);

                auto end = get_time::now();

                cout << "✔" << endl;
                cout << "HashTable size: " << table->getsize() << endl;

                auto diff = end - start;
                cout << "Elapsed time is :  " << chrono::duration_cast<ns>(diff).count() << " ms " << endl;
            }
            else
            {
                cout << "Empty HashTable created!" << endl;
                keys.changeSize(0);
            }

            deleted = false;
        }
        else if(option == OPTION2)
        {
            if(deleted == true)
                cout << "No HashTable found! Create a new one." << endl;
            else
            {
                for(int i = 0; i < keys.size(); i++)
                {
                    cout << i << " key: " << keys.get(i) << endl;
                }
                cout << endl;
                cout << "HashTable size: " << table->getsize() << endl;
            }
        }
        else if(option == OPTION3)
        {
            if(deleted == true)
                cout << "No HashTable found! Create a new one." << endl;
            else
            {
                int key, tid, offset, flag = 1;

                cout << "Key: ";
                cin >> key;
                cout << "Transaction id: ";
                cin >> tid;
                cout << "Offset: ";
                cin >> offset;

                for(int i = 0; i < keys.size(); i++)
                {
                    if(keys.get(i) == key)
                        flag = 0;
                }
                if(flag)
                    keys.push_back(key);

                auto start = get_time::now();

                table->insert(key, tid, offset);

                auto end = get_time::now();

                cout << "Key: " << key << " with tid: " << tid << " and offset: " << offset << " inserted!" << endl;
                cout << endl;
                cout << "HashTable size: " << table->getsize() << endl;

                auto diff = end - start;
                cout << "Elapsed time is :  " << chrono::duration_cast<ns>(diff).count() << " ms " << endl;
            }
        }
        else if(option == OPTION4)
        {
            if(deleted == true)
                cout << "No HashTable found! Create a new one." << endl;
            else
            {
                cout << "Key to delete: ";
                int dkey;
                cin >> dkey;

                if(table->deleteKey(dkey))
                {
                    cout << "Key: " << dkey << " deleted!" << endl;
                    for(int i = 0; i < keyNumber; i++)
                    {
                        if(keys.get(i) == dkey)
                            keys.popAt(i);
                    }
                }
                else
                    cout << "Key: " << dkey << " not found!" << endl;

                    cout << endl;
                    cout << "HashTable size: " << table->getsize() << endl;
            }
        }
        else if(option == OPTION5)
        {
            int key, of;
            cout << "Key: ";
            cin >> key;

            auto start = get_time::now();
            of = table->getLastJournalInsert(key);
            auto end = get_time::now();

            cout << "Offset: " << of << endl;

            auto diff = end - start;
            cout << "Elapsed time is :  " << chrono::duration_cast<ns>(diff).count() << " ms " << endl;
        }
        else if(option == OPTION6)  // TODO: Check the seg fault
        {
            if(deleted == true)
                cout << "No HashTable found! Create a new one." << endl;
            else
            {
                int key, start_tid, end_tid;
                DArray<int>* array = NULL;

                cout << "Key: ";
                cin >> key;
                cout << "Starting tid (-1 = return all offsets): ";
                cin >> start_tid;
                if(start_tid >= 0)
                {
                    cout << "Ending tid: ";
                    cin >> end_tid;

                    cout << endl;
                    cout << "Offsets:" << endl;

                    if(true)
                    {
                        auto start = get_time::now();
                        array = table->getHashRecord(key, start_tid, end_tid);
                        auto end = get_time::now();

                        for(int i = 0; i < array->size(); i++)
                        {
                            cout << array->get(i) <<endl;
                        }
                        cout << endl;

                        auto diff = end - start;
                        cout << "Elapsed time is :  " << chrono::duration_cast<ns>(diff).count() << " ms " << endl;
                    }
                    else
                    cout << "Key not found!" << endl;
                }
                else
                {
                    cout << "Offsets: " << endl;

                    if(true)
                    {
                        auto start = get_time::now();
                        array = table->getHashRecords(key);
                        auto end = get_time::now();

                        for(int i = 0; i < array->size(); i++)
                        {
                            cout << array->get(i) <<endl;
                        }
                        cout << endl;

                        auto diff = end - start;
                        cout << "Elapsed time is :  " << chrono::duration_cast<ns>(diff).count() << " ms " << endl;
                    }
                    else
                    cout << "Key not found!" << endl;
                }
            }
        }
        else if(option == OPTION7)
        {
            if(deleted == true)
                cout << "No HashTable found! Create a new one." << endl;
            else
            {
                cout << "Deleting keys... ";

                auto start = get_time::now();

                for(int i = 0; i < keys.size(); i++)
                {
                    table->deleteKey(keys.get(i));
                }
                auto end = get_time::now();
                cout << "✔" << endl;

                auto diff = end - start;
                cout << "Elapsed time is :  " << chrono::duration_cast<ns>(diff).count() << " ms " << endl;
            }
        }
        else if(option == OPTION8)
        {
            if(deleted == false)
                delete table;
            return 0;
        }
    }
}
