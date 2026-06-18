#include "Bookdatabase.h"
#include "User.h"
#include <algorithm>

BookDatabase& BookDatabase::instance() {
    static BookDatabase db;
    return db;
}

bool BookDatabase::addBook(const Book& book) {
    if (exists(book.title)) return false;
    books_.push_back(book);
    return true;
}

Book* BookDatabase::find(const std::string& title) {
    for (auto& b : books_)
        if (b.title == title) return &b;
    return nullptr;
}

const Book* BookDatabase::find(const std::string& title) const {
    for (const auto& b : books_)
        if (b.title == title) return &b;
    return nullptr;
}

bool BookDatabase::exists(const std::string& title) const {
    return find(title) != nullptr;
}

void BookDatabase::removeBook(const std::string& title) {
    books_.erase(
        std::remove_if(books_.begin(), books_.end(),
            [&](const Book& b) { return b.title == title; }),
        books_.end());
}

std::vector<const Book*> BookDatabase::search(const std::string& query) const {
    std::string q = User::toLower(query);
    std::vector<const Book*> results;
    for (const auto& book : books_) {
        std::string t = User::toLower(book.title);
        if (t.find(q) != std::string::npos || User::levenshtein(q, t) <= 2)
            results.push_back(&book);
    }
    return results;
}