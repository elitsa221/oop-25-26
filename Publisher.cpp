#include "Publisher.h"
#include <algorithm>

Publisher::Publisher(const std::string& u, const std::string& p, const Date& reg)
    : User(u, p, reg) {}

void Publisher::receiveMessage(const Message& msg) {
    inbox.push_back(msg);
}

void Publisher::addAuthor(const std::string& name) {
    if (!worksWithAuthor(name))
        authorNames.push_back(name);
}

void Publisher::addBook(const std::string& title) {
    if (std::find(publishedBooks.begin(), publishedBooks.end(), title) == publishedBooks.end())
        publishedBooks.push_back(title);
}

bool Publisher::worksWithAuthor(const std::string& name) const {
    return std::find(authorNames.begin(), authorNames.end(), name) != authorNames.end();
}