#ifndef BUCKET_HPP
#define BUCKET_HPP

class HashTable;


class Bucket
{
    friend class HashTable;
private:


protected:
    unsigned int key;
    int data;
    unsigned int localDepth;
    bool empty;

    Bucket();

    ~Bucket();

public:

    void insert(unsigned int key, int data);

};

#endif
