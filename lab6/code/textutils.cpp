#include "textutils.h"
#include <iostream>
#include <fstream>

// Convert a single UTF-8 Russian uppercase letter to lowercase
// Russian UTF-8: A-Ya is 0xD090-0xD0AF (and Yo=0xD001)
//                a-ya is 0xD0B0-0xD0CF + 0xD180-0xD18F (and yo=0xD191)
string toLowerRussian(const string& text) {
    string result;
    for (size_t i = 0; i < text.size(); i++) {
        unsigned char c = text[i];

        if (c == 0xD0 && i + 1 < text.size()) {
            unsigned char next = text[i + 1];
            // A(0x90) - P(0x9F) -> a(0xB0) - p(0xBF)
            if (next >= 0x90 && next <= 0x9F) {
                result += (char)0xD0;
                result += (char)(next + 0x20);
                i++;
                continue;
            }
            // R(0xA0) - Ya(0xAF) -> r(0x80) - ya(0x8F) (next byte goes to 0xD1 page)
            if (next >= 0xA0 && next <= 0xAF) {
                result += (char)0xD1;
                result += (char)(next - 0x20);
                i++;
                continue;
            }
            // Already lowercase a-p (0xB0-0xBF) -- keep as is
            if (next >= 0xB0 && next <= 0xBF) {
                result += (char)c;
                result += (char)next;
                i++;
                continue;
            }
        }

        if (c == 0xD1 && i + 1 < text.size()) {
            unsigned char next = text[i + 1];
            // Already lowercase r-ya (0x80-0x8F) -- keep as is
            if (next >= 0x80 && next <= 0x8F) {
                result += (char)c;
                result += (char)next;
                i++;
                continue;
            }
            // yo(0xD1 0x91) -- keep as is
            if (next == 0x91) {
                result += (char)c;
                result += (char)next;
                i++;
                continue;
            }
        }

        // Handle Yo -> yo (0xD0 0x81 -> 0xD1 0x91)
        if (c == 0xD0 && i + 1 < text.size() && (unsigned char)text[i + 1] == 0x81) {
            result += (char)0xD1;
            result += (char)0x91;
            i++;
            continue;
        }

        // Regular ASCII or other UTF-8 byte
        if (c >= 'A' && c <= 'Z') {
            result += (char)(c + 32);  // ASCII uppercase to lowercase
        } else {
            result += (char)c;
        }
    }
    return result;
}

// Check if a UTF-8 byte sequence at position i is a Russian letter
bool isRussianLetter(const string& text, size_t i) {
    if (i >= text.size()) return false;
    unsigned char c = text[i];

    if (c == 0xD0 && i + 1 < text.size()) {
        unsigned char next = text[i + 1];
        // A-ya range and Yo
        if ((next >= 0x90 && next <= 0xBF) || next == 0x81)
            return true;
    }
    if (c == 0xD1 && i + 1 < text.size()) {
        unsigned char next = text[i + 1];
        // r-ya and yo
        if ((next >= 0x80 && next <= 0x8F) || next == 0x91)
            return true;
    }
    return false;
}

// Check if the entire string consists only of Russian letters
bool isAllRussian(const string& text) {
    if (text.empty()) return false;
    size_t i = 0;
    while (i < text.size()) {
        if (isRussianLetter(text, i)) {
            i += 2;  // Russian letters are 2 bytes in UTF-8
        } else {
            return false;
        }
    }
    return true;
}

// Extract words from a string and insert into both dictionaries
void processText(const string& text, RBTree& tree, HashTable& hashTable) {
    string word;
    size_t i = 0;

    while (i < text.size()) {
        if (isRussianLetter(text, i)) {
            // Accumulate Russian letter (2 bytes in UTF-8)
            word += text[i];
            word += text[i + 1];
            i += 2;
        } else {
            // Non-Russian character: if we have accumulated a word, save it
            if (!word.empty()) {
                string lower = toLowerRussian(word);
                tree.insert(lower);
                hashTable.insert(lower);
                word.clear();
            }
            i++;
        }
    }
    // Don't forget the last word
    if (!word.empty()) {
        string lower = toLowerRussian(word);
        tree.insert(lower);
        hashTable.insert(lower);
    }
}

// Load text from file and insert into both dictionaries
bool loadFromFile(const string& filename, RBTree& tree, HashTable& hashTable) {
    ifstream file(filename);
    string openedFilename = filename;
    if (!file.is_open()) {
        file.clear();
        openedFilename = "../" + filename;
        file.open(openedFilename);
        if (!file.is_open()) {
            cout << "Error: cannot open file \"" << filename << "\"" << endl;
            return false;
        }
    }

    string line;
    while (getline(file, line)) {
        processText(line, tree, hashTable);
    }
    file.close();
    cout << "File \"" << openedFilename << "\" loaded successfully." << endl;
    return true;
}

// Load Pushkin's poem from a hardcoded UTF-8 string.
void loadDefaultText(RBTree& tree, HashTable& hashTable) {
    string text1 =
        "Я вас любил любовь еще быть может "
        "в душе моей угасла не совсем "
        "но пусть она вас больше не тревожит "
        "я не хочу печалить вас ничем "
        "Я вас любил безмолвно безнадежно "
        "то робостью то ревностью томим "
        "Я вас любил так искренно так нежно "
        "как дай вам бог любимой быть другим";

    processText(text1, tree, hashTable);
    cout << "Default text loaded (Пушкин - Я вас любил)." << endl;
}
