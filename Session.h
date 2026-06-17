#pragma once
#include "User.h"
#include "Date.h"

class Session {
public:
    static Session& instance();

    User* currentUser = nullptr;
    Date  today;

    bool isLoggedIn() const { return currentUser != nullptr; }
    void logout() { currentUser = nullptr; }

private:
    Session();
};

