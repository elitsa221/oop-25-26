#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "User.h"
#include "Reader.h"
#include "Author.h"
#include "Publisher.h"

class UserRegistry {
public:
    static UserRegistry& instance();
    
    bool registerUser(const std::string& username, const std::string& password,
        UserType type, const Date& today);

    User* find(const std::string& username);
    const User* find(const std::string& username) const;
    bool exists(const std::string& username) const;

    Reader* findReader(const std::string& username);
    Author* findAuthor(const std::string& username);
    Publisher* findPublisher(const std::string& username);

    std::vector<const User*> search(const std::string& query) const;

    const std::map<std::string, std::unique_ptr<User>>& all() const { return users_; }

private:
    UserRegistry() = default;
    std::map<std::string, std::unique_ptr<User>> users_;
};

