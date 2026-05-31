#include "rbtree.h"
#include <iostream>

// Create the sentinel NIL node (always BLACK)
void RBTree::initNIL() {
    NIL = new RBNode();
    NIL->word = "";
    NIL->count = 0;
    NIL->color = BLACK;
    NIL->left = nullptr;
    NIL->right = nullptr;
    NIL->parent = nullptr;
}

// Left rotation around node x
//       x            y
//      / \   =>     / \
//     a   y        x   c
//        / \      / \
//       b   c    a   b
void RBTree::rotateLeft(RBNode* x) {
    RBNode* y = x->right;      // y is x's right child
    x->right = y->left;        // move y's left subtree to x's right
    if (y->left != NIL)
        y->left->parent = x;
    y->parent = x->parent;     // link x's parent to y
    if (x->parent == nullptr)
        root = y;               // x was root, now y is root
    else if (x == x->parent->left)
        x->parent->left = y;    // x was left child
    else
        x->parent->right = y;   // x was right child
    y->left = x;               // put x on y's left
    x->parent = y;
}

// Right rotation around node x
//       x          y
//      / \  =>    / \
//     y   c      a   x
//    / \            / \
//   a   b          b   c
void RBTree::rotateRight(RBNode* x) {
    RBNode* y = x->left;       // y is x's left child
    x->left = y->right;        // move y's right subtree to x's left
    if (y->right != NIL)
        y->right->parent = x;
    y->parent = x->parent;     // link x's parent to y
    if (x->parent == nullptr)
        root = y;               // x was root
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;
    y->right = x;              // put x on y's right
    x->parent = y;
}

// Fix RB-tree properties after insertion
// New node is always RED, so we might have red-red violation
void RBTree::insertFixup(RBNode* z) {
    // While z's parent is RED (violation of property 4)
    while (z->parent != nullptr && z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            // Parent is LEFT child of grandparent
            RBNode* uncle = z->parent->parent->right;

            if (uncle->color == RED) {
                // Case 1: Uncle is RED
                // Recolor parent and uncle to BLACK, grandparent to RED
                z->parent->color = BLACK;
                uncle->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;  // move up to grandparent
            } else {
                if (z == z->parent->right) {
                    // Case 2: Uncle is BLACK, z is RIGHT child (triangle)
                    // Left rotate to convert to Case 3
                    z = z->parent;
                    rotateLeft(z);
                }
                // Case 3: Uncle is BLACK, z is LEFT child (line)
                // Right rotate grandparent, recolor
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rotateRight(z->parent->parent);
            }
        } else {
            // Parent is RIGHT child of grandparent (mirror cases)
            RBNode* uncle = z->parent->parent->left;

            if (uncle->color == RED) {
                // Case 1 mirror: Uncle is RED
                z->parent->color = BLACK;
                uncle->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    // Case 2 mirror: z is LEFT child
                    z = z->parent;
                    rotateRight(z);
                }
                // Case 3 mirror: z is RIGHT child
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rotateLeft(z->parent->parent);
            }
        }
    }
    // Root must always be BLACK
    root->color = BLACK;
}

// Replace subtree rooted at u with subtree rooted at v
void RBTree::transplant(RBNode* u, RBNode* v) {
    if (u->parent == nullptr)
        root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    v->parent = u->parent;
}

// Find the node with minimum key in subtree rooted at node
RBNode* RBTree::treeMinimum(RBNode* node) {
    while (node->left != NIL)
        node = node->left;
    return node;
}

// Fix RB-tree properties after deletion
// Called when a BLACK node was removed, causing black-height imbalance
void RBTree::deleteFixup(RBNode* x) {
    while (x != root && x->color == BLACK) {
        if (x == x->parent->left) {
            // x is LEFT child
            RBNode* w = x->parent->right;  // w is sibling

            if (w->color == RED) {
                // Case 1: Sibling is RED
                // Recolor and rotate to convert to case 2,3,4
                w->color = BLACK;
                x->parent->color = RED;
                rotateLeft(x->parent);
                w = x->parent->right;
            }

            if (w->left->color == BLACK && w->right->color == BLACK) {
                // Case 2: Sibling is BLACK, both children BLACK
                // Pull black up: make sibling RED, move x up
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    // Case 3: Sibling BLACK, left child RED, right child BLACK
                    // Rotate to convert to case 4
                    w->left->color = BLACK;
                    w->color = RED;
                    rotateRight(w);
                    w = x->parent->right;
                }
                // Case 4: Sibling BLACK, right child RED
                // Final rotation and recolor to fix everything
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                rotateLeft(x->parent);
                x = root;  // done, exit loop
            }
        } else {
            // x is RIGHT child (mirror cases)
            RBNode* w = x->parent->left;

            if (w->color == RED) {
                // Case 1 mirror
                w->color = BLACK;
                x->parent->color = RED;
                rotateRight(x->parent);
                w = x->parent->left;
            }

            if (w->right->color == BLACK && w->left->color == BLACK) {
                // Case 2 mirror
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    // Case 3 mirror
                    w->right->color = BLACK;
                    w->color = RED;
                    rotateLeft(w);
                    w = x->parent->left;
                }
                // Case 4 mirror
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rotateRight(x->parent);
                x = root;
            }
        }
    }
    x->color = BLACK;
}

// Recursively delete all nodes (post-order traversal)
void RBTree::destroyTree(RBNode* node) {
    if (node == NIL) return;
    destroyTree(node->left);
    destroyTree(node->right);
    delete node;
}

// Print tree sideways (right subtree on top, left on bottom)
// indent controls horizontal spacing for depth
void RBTree::printTreeHelper(RBNode* node, int indent) {
    if (node == NIL) {
        cout << string(indent, ' ') << "[B] NIL" << endl;
        return;
    }
    // Print right subtree first (appears on top)
    printTreeHelper(node->right, indent + 6);
    // Print current node with indentation and color
    cout << string(indent, ' ')
         << (node->color == RED ? "[R] " : "[B] ")
         << node->word << "(" << node->count << ")" << endl;
    // Print left subtree (appears on bottom)
    printTreeHelper(node->left, indent + 6);
}

// In-order traversal: prints words in sorted order
void RBTree::inOrderHelper(RBNode* node) {
    if (node == NIL) return;
    inOrderHelper(node->left);
    cout << "  " << node->word << " : " << node->count << endl;
    inOrderHelper(node->right);
}

// Count total nodes
int RBTree::countNodes(RBNode* node) {
    if (node == NIL) return 0;
    return 1 + countNodes(node->left) + countNodes(node->right);
}

RBTree::RBTree() {
    initNIL();
    root = NIL;
}

RBTree::~RBTree() {
    clear();
    delete NIL;
}

// INSERT a word into the RB-tree
// If word exists, just increment its count
void RBTree::insert(const string& word) {
    // First check if word already exists
    RBNode* existing = searchNode(word);
    if (existing != NIL) {
        existing->count++;  // word already in dictionary, increment count
        return;
    }

    // Create new node (always RED initially)
    RBNode* z = new RBNode();
    z->word = word;
    z->count = 1;
    z->color = RED;
    z->left = NIL;
    z->right = NIL;
    z->parent = nullptr;

    // Standard BST insertion: find the correct position
    RBNode* y = nullptr;   // will be parent of new node
    RBNode* x = root;      // start from root

    while (x != NIL) {
        y = x;
        if (word < x->word)
            x = x->left;    // go left if smaller
        else
            x = x->right;   // go right if larger
    }

    z->parent = y;
    if (y == nullptr)
        root = z;            // tree was empty, z is root
    else if (word < y->word)
        y->left = z;        // attach as left child
    else
        y->right = z;       // attach as right child

    // If z is root, just color it BLACK and done
    if (z->parent == nullptr) {
        z->color = BLACK;
        return;
    }

    // If grandparent is null, no fixup needed
    if (z->parent->parent == nullptr)
        return;

    // Fix any RB-tree violations caused by inserting RED node
    insertFixup(z);
}

// SEARCH for a word, returns pointer to node (or NIL if not found)
RBNode* RBTree::searchNode(const string& word) {
    RBNode* current = root;
    while (current != NIL) {
        if (word == current->word)
            return current;      // found it
        else if (word < current->word)
            current = current->left;   // go left
        else
            current = current->right;  // go right
    }
    return NIL;  // not found
}

// Public search with output
bool RBTree::search(const string& word) {
    RBNode* result = searchNode(word);
    if (result != NIL) {
        cout << "Found: \"" << result->word
             << "\" (count: " << result->count << ")" << endl;
        return true;
    }
    cout << "Word \"" << word << "\" not found in dictionary." << endl;
    return false;
}

// DELETE a word from the RB-tree
bool RBTree::remove(const string& word) {
    // Find the node to delete
    RBNode* z = searchNode(word);
    if (z == NIL) {
        cout << "Word \"" << word << "\" not found, nothing to delete." << endl;
        return false;
    }

    RBNode* y = z;           // y is the node that will actually be removed
    RBNode* x;               // x is the child that replaces y
    Color yOriginalColor = y->color;  // remember color for fixup

    if (z->left == NIL) {
        // Case 1: no left child - replace z with its right child
        x = z->right;
        transplant(z, z->right);
    } else if (z->right == NIL) {
        // Case 2: no right child - replace z with its left child
        x = z->left;
        transplant(z, z->left);
    } else {
        // Case 3: two children
        // Find in-order successor (smallest in right subtree)
        y = treeMinimum(z->right);
        yOriginalColor = y->color;
        x = y->right;

        if (y->parent == z) {
            // Successor is direct right child of z
            x->parent = y;
        } else {
            // Successor is deeper in the right subtree
            transplant(y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        // Replace z with its successor y
        transplant(z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;  // keep z's color
    }

    delete z;  // free memory

    // If we removed a BLACK node, the black-height is broken - fix it
    if (yOriginalColor == BLACK)
        deleteFixup(x);

    return true;
}

// CLEAR the entire dictionary
void RBTree::clear() {
    if (root != NIL) {
        destroyTree(root);
        root = NIL;
    }
}

// Print dictionary in sorted order (in-order traversal)
void RBTree::printDictionary() {
    if (root == NIL) {
        cout << "Dictionary is empty." << endl;
        return;
    }
    cout << "=== RB-Tree Dictionary (sorted) ===" << endl;
    inOrderHelper(root);
    cout << "Total words: " << countNodes(root) << endl;
}

// Print tree structure with colors
void RBTree::printTree() {
    if (root == NIL) {
        cout << "Tree is empty." << endl;
        return;
    }
    cout << "=== RB-Tree Structure ===" << endl;
    cout << "(read sideways: right on top, left on bottom)" << endl;
    cout << "[B] = Black, [R] = Red" << endl;
    cout << "---" << endl;
    printTreeHelper(root, 0);
    cout << "---" << endl;
}

bool RBTree::isEmpty() {
    return root == NIL;
}
