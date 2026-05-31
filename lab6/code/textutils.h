#ifndef TEXTUTILS_H
#define TEXTUTILS_H

#include <string>
#include "rbtree.h"
#include "hashtable.h"

using namespace std;

// Convert UTF-8 Russian uppercase letters to lowercase
string toLowerRussian(const string& text);

// Check if a UTF-8 byte sequence at position i is a Russian letter
bool isRussianLetter(const string& text, size_t i);

// Check if the entire string consists only of Russian letters
bool isAllRussian(const string& text);

// Extract words from a string and insert into both dictionaries
void processText(const string& text, RBTree& tree, HashTable& hashTable);

// Load text from file and insert into both dictionaries
bool loadFromFile(const string& filename, RBTree& tree, HashTable& hashTable);

// Load text from a hardcoded string (for demo without file)
void loadDefaultText(RBTree& tree, HashTable& hashTable);

#endif
