#include "queryClass.hpp"

using namespace std;

string stringBuilder(int start, int end, int col, int op, uint64_t value){
    string key;

    key = to_string(start) + "-" + to_string(end) + "c" + to_string(col);

    switch(op){
        case Equal: key += "="; break;
        case NotEqual: key += "!"; break;
        case Less: key += "<"; break;
        case LessOrEqual: key += "["; break;
        case Greater: key += ">"; break;
        case GreaterOrEqual: key += "]"; break;
    }

    key += to_string(value);

    return key;
}

QueryClass::~QueryClass()
{
    for(unsigned i = 0; i < columnCount; i++)
        delete columns[i];

    delete[] columns;
}

QueryClass::QueryClass(const uint32_t &relId, const uint32_t &colCount, ColumnPtr * cols)
{
    relationId = relId;
    columnCount = colCount;
    columns = cols;
}

bool QueryClass::validate(uint64_t &from, uint64_t &to)
{
    bool conflict = true;

    //==========================================================
    // 1os elegxos: An einai valid ta predicates twn subqueries
    // Arxikopoihsh twn priority tables
    //==========================================================

    DArray<ColumnPtr>* priority1 = new DArray<ColumnPtr>(10);        // subqueries me c0 =
    DArray<ColumnPtr>* priority2 = new DArray<ColumnPtr>(10);        // subqueries me =
    DArray<ColumnPtr>* priority3 = new DArray<ColumnPtr>(10);        // ola ta upoloipa

    // iterate over subqueries
    for (unsigned w = 0; w < columnCount; w++)
    {
        ColumnClass *c = columns[w];
        // ean to operation einai '=' kai anaferetai sto primary key (c0)
        if (c->op == Equal && c->column == 0)
        {
            if(priority1->size() == 0)
                priority1->push_back(c);
            else
            {
                if(priority1->get(0)->value != c->value)  // An uparxei hdh subquery me diaforetikh timh sto c0 =
                {
                    // Go to the next query
                    conflict = false;   // Se periptwsh poy den uparxei allo query
                    break;				// no need to check the next queries for this validation
                }
            }
        }
        else if(c->op == Equal)
            priority2->push_back(c);
        else
            priority3->push_back(c);
    }

    if(conflict == false)   // Sthn periptvsh poy vgei false apo ton 1o elegxo
    {
        delete priority1;
        delete priority2;
        delete priority3;
        return conflict;
    }

    //================================================================
    // 2os elegxos: Filtrarisma twn records me vash ta priority tables
    //================================================================

    DArray<JournalRecord*>* records_to_check = NULL;    // DArray me ta journal records poy exoume na elegksoume

    unsigned psize1 = priority1->size();
    if(psize1 == 1)
    {
        records_to_check = Journals[relationId]->getRecordsbyKey(priority1->get(0)->value, from, to);
        if(records_to_check == NULL)
        {
            delete priority1;
            delete priority2;
            delete priority3;
            return false;
        }
    }
    else
    {
        records_to_check = Journals[relationId]->getJournalRecords(from, to);
    }

    unsigned psize2 = priority2->size();
    for(unsigned j = 0; j < psize2; j++)
    {
        int rtcsize = records_to_check->size();
        for(int i = 0; i < rtcsize; i++)
        {
            if(priority2->get(j)->value != records_to_check->get(i)->getValue(priority2->get(j)->column))
            {
                records_to_check->fastPopAt(i);
                i--;
                rtcsize--;
            }
        }
        if(rtcsize == 0)
        {
            delete priority1;
            delete priority2;
            delete priority3;
            delete records_to_check;
            return false;
        }
    }

    unsigned psize3 = priority3->size();
    for(unsigned j = 0; j < psize3; j++)
    {
        int rtcsize = records_to_check->size();
        for(int i = 0; i < rtcsize; i++)
        {
            uint64_t query_value = priority3->get(j)->value;
            uint64_t tuple_value = records_to_check->get(i)->getValue(priority3->get(j)->column);
            bool result;

            switch (priority3->get(j)->op)
            {
                case NotEqual: 	result=(tuple_value != query_value); break;
                case Less: 		result=(tuple_value < query_value); break;
                case LessOrEqual: result=(tuple_value <= query_value); break;
                case Greater: 	result=(tuple_value > query_value); break;
                case GreaterOrEqual: result=(tuple_value >= query_value); break;
                default: result = false ; break;
            }

            if(result == false)
            {
                records_to_check->fastPopAt(i);
                i--;
                rtcsize--;
            }
        }
        if(rtcsize == 0)
        {
            delete priority1;
            delete priority2;
            delete priority3;
            delete records_to_check;
            return false;
        }
    }

    delete priority1;
    delete priority2;
    delete priority3;
    delete records_to_check;

    return true;
}

#if VAL_HASHTABLE == 1
bool QueryClass::hashValidate(uint64_t &from, uint64_t &to)
{
    bool conflict = true;

    //==========================================================
    // 1os elegxos: An einai valid ta predicates twn subqueries
    // Arxikopoihsh twn priority tables
    //==========================================================

    DArray<ColumnPtr>* priority2 = new DArray<ColumnPtr>(10);        // subqueries me =
    DArray<ColumnPtr>* priority3 = new DArray<ColumnPtr>(10);        // ola ta upoloipa

    // iterate over subqueries
    for (unsigned w = 0; w < columnCount; w++)
    {
        ColumnClass *c = columns[w];
        // ean to operation einai '=' kai anaferetai sto primary key (c0)

        if(c->op == Equal)
            priority2->push_back(c);
        else
            priority3->push_back(c);
    }

    if(conflict == false)   // Sthn periptvsh poy vgei false apo ton 1o elegxo
    {
        delete priority2;
        delete priority3;
        return conflict;
    }

    //================================================================
    // 2os elegxos: Filtrarisma twn records me vash ta priority tables
    //================================================================

    Bitset* resultBitset = NULL;
    Bitset* bit = NULL;
    int counter;

    DArray<JournalRecord*>* records_to_check = NULL;    // DArray me ta journal records poy exoume na elegksoume

    unsigned psize2 = priority2->size();
    for(unsigned j = 0; j < psize2; j++)
    {
        if(bit != NULL)
        {
            delete bit;
            bit = NULL;
        }

        bit = Journals[relationId]->val_htable.getbdata(priority2->get(j)->key, &counter);

        if(bit == NULL)	                   // not calculated case
        {
            if(records_to_check == NULL)
                records_to_check = Journals[relationId]->getJournalRecords(from, to);
            bit = priority2->get(j)->checkColumn(*records_to_check);
        }

        if(resultBitset != NULL)
            resultBitset->addBitset(bit);
        else
            resultBitset = new Bitset(*bit);

        if(!resultBitset->validateBitset())     // Doesn't exist. Delete and return
        {
            delete priority2;
            delete priority3;
            delete records_to_check;
            if(resultBitset == bit)
                delete bit;
            else {delete bit; delete resultBitset;}
            return false;
        }
        if((counter > 0) && (forget <= from))
            Journals[relationId]->val_htable.UpdateValData(priority2->get(j)->key, bit);

        if((counter == 0) || (forget > from))           // Delete key
            Journals[relationId]->val_htable.deleteKey(priority2->get(j)->key);
    }

    unsigned psize3 = priority3->size();
    for(unsigned j = 0; j < psize3; j++)
    {
        if(bit != NULL)
        {
            delete bit;
            bit = NULL;
        }
        bit = Journals[relationId]->val_htable.getbdata(priority3->get(j)->key, &counter);

        if(bit == NULL)	                   // not calculated case
        {
            if(records_to_check == NULL)
                records_to_check = Journals[relationId]->getJournalRecords(from, to);

            bit = priority3->get(j)->checkColumn(*records_to_check);
        }

        if(resultBitset != NULL)
            resultBitset->addBitset(bit);
        else
            resultBitset = new Bitset(*bit);

        if(!resultBitset->validateBitset())     // Doesn't exist. Delete and return
        {
            delete priority2;
            delete priority3;
            delete records_to_check;
            if(resultBitset == bit)
                delete bit;
            else {delete bit; delete resultBitset;}
            return false;
        }
        if((counter > 0) && (forget <= from))
            Journals[relationId]->val_htable.UpdateValData(priority3->get(j)->key, bit);

        if((counter == 0) || (forget > from))          // Delete key
            Journals[relationId]->val_htable.deleteKey(priority3->get(j)->key);
    }

    delete priority2;
    delete priority3;
    delete records_to_check;

    if(resultBitset == bit)
        delete bit;
    else
    {
        delete resultBitset;
        delete bit;
    }

    return true;
}

#endif
