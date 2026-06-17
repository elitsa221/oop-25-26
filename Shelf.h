#pragma once
#include <string>
#include <vector>
#include "Date.h"
class Shelf
{
public:
    std::string name;
    std::vector<std::string> bookTitles;   
    Date createdDate;

    Shelf() = default;
    Shelf(const std::string& name, const Date& created)
        : name(name), createdDate(created) {}

    bool addBook(const std::string& title);
    bool removeBook(const std::string& title);
    bool hasBook(const std::string& title) const;
    int bookCount() const { return (int)bookTitles.size(); }
};

