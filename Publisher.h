#pragma once
#include "User.h"
#include "Message.h"
#include <vector>
#include <string>
class Publisher : public User {
public:
    std::vector<std::string> authorNames;
    std::vector<std::string> publishedBooks;
    std::vector<Message> inbox;

    Publisher() = default;
    Publisher(const std::string& username, const std::string& password, const Date& reg);

    UserType type() const override { return UserType::PUBLISHER; }
    std::string typeString() const override { return "Publisher"; }

    void receiveMessage(const Message& msg);
    void addAuthor(const std::string& name);
    void addBook(const std::string& title);
    bool worksWithAuthor(const std::string& name) const;
};

