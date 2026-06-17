#pragma once
#include <string>
#include <vector>
#include "Date.h"
#include "Message.h"

enum class UserType { READER, AUTHOR, PUBLISHER };
class User
{
public:
    std::string username;
    std::string password;
    Date registrationDate;
    std::vector<std::string> followers;   
    std::vector<std::string> following;   

    User() = default;
    User(const std::string& username, const std::string& password, const Date& reg);
    virtual ~User() = default;

    virtual UserType type() const = 0;
    virtual std::string typeString() const = 0;

    bool addFollower(const std::string& username);
    bool removeFollower(const std::string& username);
    bool isFollowedBy(const std::string& username) const;
    bool isFollowing(const std::string& username) const;
    
    static bool validatePassword(const std::string& pw);
    static bool validateUsername(const std::string& un);
    
    static int levenshtein(const std::string& a, const std::string& b);
    static std::string toLower(const std::string& s);
};

