#include "hashtable.h"
#include <iostream>

// Polynomial rolling hash function
// h(s) = (s[0]*31^(n-1) + s[1]*31^(n-2) + ... + s[n-1]) mod TABLE_SIZE
//
// Why 31?
// 1) 31 is a prime number - primes give better distribution and fewer collisions
//    because they have no common factors with typical data patterns
// 2) 31 = 2^5 - 1, so multiplication by 31 can be optimized by the compiler
//    to (hash << 5) - hash, which is faster than general multiplication
// 3) 31 is small enough to avoid integer overflow for short strings,
//    but large enough to spread hash values across the table
// 4) This is the same constant used in Java's String.hashCode() and
//    many other well-tested hash implementations
// 5) Odd primes (like 31) work better than even numbers because
//    multiplying by an even number shifts bits left and loses information
//
// Why TABLE_SIZE = 127 (prime)?
// A prime table size ensures that (hash % TABLE_SIZE) uses all buckets
// evenly, since gcd(31, 127) = 1 (no common factors -> fewer clusters)
unsigned int HashTable::hashFunction(const string& word) {
    unsigned int hash = 0;
    for (size_t i = 0; i < word.length(); i++) {
        hash = hash * 31 + (unsigned char)word[i];
    }
    return hash % TABLE_SIZE;
}

HashTable::HashTable() {
    // Initialize all buckets to empty
    for (int i = 0; i < TABLE_SIZE; i++)
        table[i] = nullptr;
}

HashTable::~HashTable() {
    clear();
}

// INSERT a word into hash table
void HashTable::insert(const string& word) {
    unsigned int index = hashFunction(word);

    // Check if word already exists in this bucket's chain
    HashNode* current = table[index];
    while (current != nullptr) {
        if (current->word == word) {
            current->count++;  // already exists, increment count
            return;
        }
        current = current->next;
    }

    // Word not found, create new node at head of chain
    HashNode* newNode = new HashNode();
    newNode->word = word;
    newNode->count = 1;
    newNode->next = table[index];  // insert at front of list
    table[index] = newNode;
}

// SEARCH for a word
bool HashTable::search(const string& word) {
    unsigned int index = hashFunction(word);

    // Walk the chain at this bucket
    HashNode* current = table[index];
    while (current != nullptr) {
        if (current->word == word) {
            cout << "Found: \"" << current->word
                 << "\" (count: " << current->count
                 << ", bucket: " << index << ")" << endl;
            return true;
        }
        current = current->next;
    }
    cout << "Word \"" << word << "\" not found in hash table." << endl;
    return false;
}

// DELETE a word from hash table
bool HashTable::remove(const string& word) {
    unsigned int index = hashFunction(word);

    HashNode* current = table[index];
    HashNode* prev = nullptr;

    while (current != nullptr) {
        if (current->word == word) {
            // Found the node to delete
            if (prev == nullptr)
                table[index] = current->next;  // removing head of chain
            else
                prev->next = current->next;    // bypass the node
            delete current;
            cout << "Deleted \"" << word << "\" from bucket " << index << endl;
            return true;
        }
        prev = current;
        current = current->next;
    }
    cout << "Word \"" << word << "\" not found, nothing to delete." << endl;
    return false;
}

// CLEAR the entire hash table
void HashTable::clear() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode* current = table[i];
        while (current != nullptr) {
            HashNode* temp = current;
            current = current->next;
            delete temp;
        }
        table[i] = nullptr;
    }
}

// Print all words in the hash table
// Collisions are shown as chains on one line: [44] word1(2) -> word2(1) [collision]
void HashTable::printDictionary() {
    cout << "=== Hash Table Dictionary ===" << endl;
    int totalWords = 0;
    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode* current = table[i];
        if (current == nullptr) continue;

        int chainLen = 0;
        cout << "  [" << i << "] ";
        while (current != nullptr) {
            if (chainLen > 0) cout << " -> ";
            cout << current->word << "(" << current->count << ")";
            chainLen++;
            totalWords++;
            current = current->next;
        }
        if (chainLen > 1) cout << "  [collision]";
        cout << endl;
    }
    if (totalWords == 0)
        cout << "Dictionary is empty." << endl;
    else
        cout << "Total unique words: " << totalWords << endl;
}

// Print statistics: bucket usage, collisions, load factor
void HashTable::printStats() {
    cout << "=== Hash Table Statistics ===" << endl;
    int usedBuckets = 0;
    int totalWords = 0;
    int maxChain = 0;
    int collisions = 0;

    for (int i = 0; i < TABLE_SIZE; i++) {
        int chainLen = 0;
        HashNode* current = table[i];
        while (current != nullptr) {
            chainLen++;
            current = current->next;
        }
        if (chainLen > 0) usedBuckets++;
        if (chainLen > 1) collisions += (chainLen - 1);
        if (chainLen > maxChain) maxChain = chainLen;
        totalWords += chainLen;
    }

    cout << "Table size: " << TABLE_SIZE << endl;
    cout << "Used buckets: " << usedBuckets << "/" << TABLE_SIZE << endl;
    cout << "Total unique words: " << totalWords << endl;
    cout << "Load factor: " << (double)totalWords / TABLE_SIZE << endl;
    cout << "Total collisions: " << collisions << endl;
    cout << "Max chain length: " << maxChain << endl;
    cout << "Hash function: polynomial (base=31, mod=" << TABLE_SIZE << ")" << endl;
}

bool HashTable::isEmpty() {
    for (int i = 0; i < TABLE_SIZE; i++)
        if (table[i] != nullptr) return false;
    return true;
}
