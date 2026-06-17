#pragma once
#include "User.h"
#include "Shelf.h"
#include "Message.h"
#include <map>
#include <optional>

enum class ReadStatus { PLAN_TO_READ, READING, PAUSED, DROPPED };

struct BookEntry {
    std::string title;
    ReadStatus status;
    double rating = -1.0;   
};

ReadStatus statusFromString(const std::string& s);
std::string statusToString(ReadStatus s);

class Reader : public User
{
public:
    std::vector<BookEntry> books;
    std::vector<Shelf> shelves;
    std::vector<std::string> favoriteBooks;
    std::vector<Message> inbox;
    Date birthday;
    bool hasBirthday = false;

    Reader() = default;
    Reader(const std::string& username, const std::string& password, const Date& reg);

    UserType type() const override { return UserType::READER; }
    std::string typeString() const override { return "Reader"; }

    void receiveMessage(const Message& msg);

    
    bool hasBook(const std::string& title) const;
    BookEntry* findBook(const std::string& title);
    bool addBook(const std::string& title, ReadStatus status, double rating = -1.0);
    bool removeBook(const std::string& title);  

    
    Shelf* findShelf(const std::string& name);
    bool createShelf(const std::string& name, const Date& today);
    bool deleteShelf(const std::string& name);

    
    bool isFriendWith(const User* other) const;
};

