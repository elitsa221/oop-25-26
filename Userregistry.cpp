#include "Userregistry.h"
#include <stdexcept>

UserRegistry& UserRegistry::instance() {
    static UserRegistry reg;
    return reg;
}

bool UserRegistry::registerUser(const std::string& username, const std::string& password,
    UserType type, const Date& today) {
    if (exists(username)) return false;
    std::unique_ptr<User> u;
    switch (type) {
    case UserType::READER:    u = std::make_unique<Reader>(username, password, today);    break;
    case UserType::AUTHOR:    u = std::make_unique<Author>(username, password, today);    break;
    case UserType::PUBLISHER: u = std::make_unique<Publisher>(username, password, today); break;
    }
    users_.push_back(std::move(u));
    return true;
}

User* UserRegistry::find(const std::string& username) {
    for (auto& u : users_)
        if (u->username == username) return u.get();
    return nullptr;
}

const User* UserRegistry::find(const std::string& username) const {
    for (const auto& u : users_)
        if (u->username == username) return u.get();
    return nullptr;
}

bool UserRegistry::exists(const std::string& username) const {
    return find(username) != nullptr;
}

Reader* UserRegistry::findReader(const std::string& username) {
    return dynamic_cast<Reader*>(find(username));
}

Author* UserRegistry::findAuthor(const std::string& username) {
    return dynamic_cast<Author*>(find(username));
}

Publisher* UserRegistry::findPublisher(const std::string& username) {
    return dynamic_cast<Publisher*>(find(username));
}

std::vector<const User*> UserRegistry::search(const std::string& query) const {
    std::string q = User::toLower(query);
    std::vector<const User*> results;
    for (const auto& u : users_) {
        std::string n = User::toLower(u->username);
        if (n.find(q) != std::string::npos || User::levenshtein(q, n) <= 2)
            results.push_back(u.get());
    }
    return results;
}