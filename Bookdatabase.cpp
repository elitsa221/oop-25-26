#include "Bookdatabase.h"
#include "User.h"

BookDatabase& BookDatabase::instance() {
    static BookDatabase db;
    return db;
}

bool BookDatabase::addBook(const Book& book) {
    if (exists(book.title)) return false;
    books_[book.title] = book;
    return true;
}

Book* BookDatabase::find(const std::string& title) {
    auto it = books_.find(title);
    return it == books_.end() ? nullptr : &it->second;
}

const Book* BookDatabase::find(const std::string& title) const {
    auto it = books_.find(title);
    return it == books_.end() ? nullptr : &it->second;
}

bool BookDatabase::exists(const std::string& title) const {
    return books_.count(title) > 0;
}

void BookDatabase::removeBook(const std::string& title) {
    books_.erase(title);
}

std::vector<const Book*> BookDatabase::search(const std::string& query) const {
    std::string q = User::toLower(query);
    std::vector<const Book*> results;
    for (auto& [title, book] : books_) {
        std::string t = User::toLower(title);        
        if (t.find(q) != std::string::npos || User::levenshtein(q, t) <= 2)
            results.push_back(&book);
    }
    return results;
}