#pragma once
#include <map>
#include <string>
#include <vector>
#include "Book.h"

class BookDatabase {
public:
    static BookDatabase& instance();

    bool addBook(const Book& book);
    Book* find(const std::string& title);
    const Book* find(const std::string& title) const;
    bool exists(const std::string& title) const;
    void removeBook(const std::string& title);

    
    std::vector<const Book*> search(const std::string& query) const;

    const std::map<std::string, Book>& all() const { return books_; }

private:
    BookDatabase() = default;
    std::map<std::string, Book> books_;   
};

