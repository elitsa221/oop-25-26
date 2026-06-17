#include "Reader.h"
#include <algorithm>
#include <stdexcept>
ReadStatus statusFromString(const std::string& s) {
    if (s == "plan-to-read") return ReadStatus::PLAN_TO_READ;
    if (s == "reading")      return ReadStatus::READING;
    if (s == "paused")       return ReadStatus::PAUSED;
    if (s == "dropped")      return ReadStatus::DROPPED;
    throw std::invalid_argument("Unknown status: " + s);
}

std::string statusToString(ReadStatus s) {
    switch (s) {
    case ReadStatus::PLAN_TO_READ: return "plan-to-read";
    case ReadStatus::READING:      return "reading";
    case ReadStatus::PAUSED:       return "paused";
    case ReadStatus::DROPPED:      return "dropped";
    }
    return "";
}

Reader::Reader(const std::string& u, const std::string& p, const Date& reg)
    : User(u, p, reg) {}

void Reader::receiveMessage(const Message& msg) {
    inbox.push_back(msg);
}

bool Reader::hasBook(const std::string& title) const {
    return std::any_of(books.begin(), books.end(),
        [&](const BookEntry& b) { return b.title == title; });
}

BookEntry* Reader::findBook(const std::string& title) {
    auto it = std::find_if(books.begin(), books.end(),
        [&](const BookEntry& b) { return b.title == title; });
    return it == books.end() ? nullptr : &(*it);
}

bool Reader::addBook(const std::string& title, ReadStatus status, double rating) {
    if (hasBook(title)) return false;
    books.push_back({ title, status, rating });
    return true;
}

bool Reader::removeBook(const std::string& title) {
    auto it = std::find_if(books.begin(), books.end(),
        [&](const BookEntry& b) { return b.title == title; });
    if (it == books.end()) return false;
    books.erase(it);
    // Remove from all shelves
    for (auto& shelf : shelves)
        shelf.removeBook(title);
    // Remove from favorites
    favoriteBooks.erase(
        std::remove(favoriteBooks.begin(), favoriteBooks.end(), title),
        favoriteBooks.end());
    return true;
}

Shelf* Reader::findShelf(const std::string& name) {
    auto it = std::find_if(shelves.begin(), shelves.end(),
        [&](const Shelf& s) { return s.name == name; });
    return it == shelves.end() ? nullptr : &(*it);
}

bool Reader::createShelf(const std::string& name, const Date& today) {
    if (findShelf(name)) return false;
    shelves.emplace_back(name, today);
    return true;
}

bool Reader::deleteShelf(const std::string& name) {
    auto it = std::find_if(shelves.begin(), shelves.end(),
        [&](const Shelf& s) { return s.name == name; });
    if (it == shelves.end()) return false;
    shelves.erase(it);
    return true;
}

bool Reader::isFriendWith(const User* other) const {
    return isFollowing(other->username) && other->isFollowedBy(username);
}