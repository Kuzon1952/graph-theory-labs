#include <iostream>
#include <string>
#include <sstream>
#include "rbtree.h"
#include "hashtable.h"
#include "textutils.h"

using namespace std;

int main() {
    // Set console to UTF-8 on Windows
    #ifdef _WIN32
    system("chcp 65001 > nul");
    #endif

    RBTree rbTree;
    HashTable hashTable;
    string input;
    string choiceStr;
    int choice;

    cout << "==========================================================" << endl;
    cout << "  Lab 6: Dictionary (RB-Tree + Hash Table)" << endl;
    cout << "==========================================================" << endl;

    while (true) {
        cout << "\n--- Main Menu ---" << endl;
        cout << "1.  Load default text (\xd0\x9f\xd1\x83\xd1\x88\xd0\xba\xd0\xb8\xd0\xbd - \xd0\xaf \xd0\xb2\xd0\xb0\xd1\x81 \xd0\xbb\xd1\x8e\xd0\xb1\xd0\xb8\xd0\xbb)" << endl;
        cout << "2.  Load text from file" << endl;
        cout << "--- Red-Black Tree ---" << endl;
        cout << "3.  Add word (RB-Tree)" << endl;
        cout << "4.  Search word (RB-Tree)" << endl;
        cout << "5.  Delete word (RB-Tree)" << endl;
        cout << "6.  Print dictionary sorted (RB-Tree)" << endl;
        cout << "7.  Print tree structure (RB-Tree)" << endl;
        cout << "8.  Clear dictionary (RB-Tree)" << endl;
        cout << "--- Hash Table ---" << endl;
        cout << "9.  Add word (Hash Table)" << endl;
        cout << "10. Search word (Hash Table)" << endl;
        cout << "11. Delete word (Hash Table)" << endl;
        cout << "12. Print dictionary (Hash Table)" << endl;
        cout << "13. Print statistics (Hash Table)" << endl;
        cout << "14. Clear dictionary (Hash Table)" << endl;
        cout << "---" << endl;
        cout << "0.  Exit" << endl;
        cout << "Choice: ";

        if (!(cin >> choiceStr)) {
            cout << "Input error. Exiting." << endl;
            return 0;
        }

        // Validate that input is a number
        bool validNumber = true;
        for (size_t i = 0; i < choiceStr.size(); i++) {
            if (!isdigit((unsigned char)choiceStr[i])) {
                validNumber = false;
                break;
            }
        }
        if (!validNumber || choiceStr.empty()) {
            cout << "Error: please enter a number (0-14)." << endl;
            continue;
        }

        choice = stoi(choiceStr);
        if (choice < 0 || choice > 14) {
            cout << "Error: please enter a number from 0 to 14." << endl;
            continue;
        }

        switch (choice) {
            case 0:
                cout << "Goodbye!" << endl;
                return 0;

            case 1:
                loadDefaultText(rbTree, hashTable);
                break;

            case 2: {
                cout << "Enter filename: ";
                cin >> input;
                loadFromFile(input, rbTree, hashTable);
                break;
            }

            case 3: {
                cout << "Enter word to add (Russian only): ";
                cin >> input;
                if (!isAllRussian(input)) {
                    cout << "Error: only Russian letters are allowed." << endl;
                    break;
                }
                input = toLowerRussian(input);
                rbTree.insert(input);
                cout << "Word \"" << input << "\" added to RB-Tree." << endl;
                cout << "\nTree after insertion:" << endl;
                rbTree.printTree();
                break;
            }

            case 4: {
                cout << "Enter word to search (Russian only): ";
                cin >> input;
                if (!isAllRussian(input)) {
                    cout << "Error: only Russian letters are allowed." << endl;
                    break;
                }
                input = toLowerRussian(input);
                rbTree.search(input);
                break;
            }

            case 5: {
                cout << "Enter word to delete (Russian only): ";
                cin >> input;
                if (!isAllRussian(input)) {
                    cout << "Error: only Russian letters are allowed." << endl;
                    break;
                }
                input = toLowerRussian(input);
                cout << "\nTree BEFORE deletion:" << endl;
                rbTree.printTree();
                if (rbTree.remove(input)) {
                    cout << "\nTree AFTER deletion of \"" << input << "\":" << endl;
                    rbTree.printTree();
                }
                break;
            }

            case 6:
                rbTree.printDictionary();
                break;

            case 7:
                rbTree.printTree();
                break;

            case 8:
                rbTree.clear();
                cout << "RB-Tree dictionary cleared." << endl;
                break;

            case 9: {
                cout << "Enter word to add (Russian only): ";
                cin >> input;
                if (!isAllRussian(input)) {
                    cout << "Error: only Russian letters are allowed." << endl;
                    break;
                }
                input = toLowerRussian(input);
                hashTable.insert(input);
                cout << "Word \"" << input << "\" added to Hash Table." << endl;
                break;
            }

            case 10: {
                cout << "Enter word to search (Russian only): ";
                cin >> input;
                if (!isAllRussian(input)) {
                    cout << "Error: only Russian letters are allowed." << endl;
                    break;
                }
                input = toLowerRussian(input);
                hashTable.search(input);
                break;
            }

            case 11: {
                cout << "Enter word to delete (Russian only): ";
                cin >> input;
                if (!isAllRussian(input)) {
                    cout << "Error: only Russian letters are allowed." << endl;
                    break;
                }
                input = toLowerRussian(input);
                hashTable.remove(input);
                break;
            }

            case 12:
                hashTable.printDictionary();
                break;

            case 13:
                hashTable.printStats();
                break;

            case 14:
                hashTable.clear();
                cout << "Hash Table dictionary cleared." << endl;
                break;

            default:
                cout << "Invalid choice." << endl;
        }
    }

    return 0;
}
