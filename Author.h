#pragma once
#include "Reader.h"
#include <vector>
#include <string>
class Author : public Reader {
public:
    std::vector<std::string> publishedBooks;  
    std::vector<std::string> publisherNames; 

    Author() = default;
    Author(const std::string& username, const std::string& password, const Date& reg);

    UserType type() const override { return UserType::AUTHOR; }
    std::string typeString() const override { return "Author"; }

    void addPublishedBook(const std::string& title);
    void addPublisher(const std::string& name);
    void removePublisher(const std::string& name);
    bool worksWithPublisher(const std::string& name) const;
};

