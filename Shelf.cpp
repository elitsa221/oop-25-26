#include "Shelf.h"
#include <algorithm>

bool Shelf::addBook(const std::string& title) {
    if (hasBook(title)) return false;
    bookTitles.push_back(title);
    return true;
}

bool Shelf::removeBook(const std::string& title) {
    auto it = std::find(bookTitles.begin(), bookTitles.end(), title);
    if (it == bookTitles.end()) return false;
    bookTitles.erase(it);
    return true;
}

bool Shelf::hasBook(const std::string& title) const {
    return std::find(bookTitles.begin(), bookTitles.end(), title) != bookTitles.end();
}