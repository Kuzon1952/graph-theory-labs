#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>
using namespace std;

const int TABLE_SIZE = 127;  // prime number for better distribution

// Node for linked list in each bucket (chaining)
struct HashNode {
    string word;
    int count;
    HashNode* next;
};

class HashTable {
private:
    HashNode* table[TABLE_SIZE];

    // Polynomial rolling hash function
    // h(s) = (s[0]*31^(n-1) + s[1]*31^(n-2) + ... + s[n-1]) mod TABLE_SIZE
    // Uses ALL characters of the word (not trivial like first letter)
    // Base 31 is prime, gives good distribution for strings
    // Modulo by prime table size (127) reduces clustering
    unsigned int hashFunction(const string& word);

public:
    HashTable();
    ~HashTable();

    void insert(const string& word);
    bool search(const string& word);
    bool remove(const string& word);
    void clear();
    void printDictionary();
    void printStats();
    bool isEmpty();
};

#endif
