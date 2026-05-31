#ifndef RBTREE_H
#define RBTREE_H

#include <string>
using namespace std;

enum Color { RED, BLACK };

struct RBNode {
    string word;
    int count;
    Color color;
    RBNode* left;
    RBNode* right;
    RBNode* parent;
};

class RBTree {
private:
    RBNode* root;
    RBNode* NIL;

    void initNIL();
    void rotateLeft(RBNode* x);
    void rotateRight(RBNode* x);
    void insertFixup(RBNode* z);
    void transplant(RBNode* u, RBNode* v);
    RBNode* treeMinimum(RBNode* node);
    void deleteFixup(RBNode* x);
    void destroyTree(RBNode* node);
    void printTreeHelper(RBNode* node, int indent);
    void inOrderHelper(RBNode* node);
    int countNodes(RBNode* node);

public:
    RBTree();
    ~RBTree();

    void insert(const string& word);
    RBNode* searchNode(const string& word);
    bool search(const string& word);
    bool remove(const string& word);
    void clear();
    void printDictionary();
    void printTree();
    bool isEmpty();
};

#endif
