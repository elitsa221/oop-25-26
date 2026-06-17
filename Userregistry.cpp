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
    users_[username] = std::move(u);
    return true;
}

User* UserRegistry::find(const std::string& username) {
    auto it = users_.find(username);
    return it == users_.end() ? nullptr : it->second.get();
}

const User* UserRegistry::find(const std::string& username) const {
    auto it = users_.find(username);
    return it == users_.end() ? nullptr : it->second.get();
}

bool UserRegistry::exists(const std::string& username) const {
    return users_.count(username) > 0;
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
    for (auto& [name, user] : users_) {
        std::string n = User::toLower(name);
        if (n.find(q) != std::string::npos || User::levenshtein(q, n) <= 2)
            results.push_back(user.get());
    }
    return results;
}