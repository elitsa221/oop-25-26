#pragma once
#include "Command.h"
#include "Session.h"
#include "UserRegistry.h"
#include "BookDatabase.h"
#include <sstream>
#include <iomanip>


class RegisterCommand : public Command {
public:
    std::string name() const override { return "register"; }
    std::string helpText() const override {
        return "register <username> <password> <type>  — type: reader/author/publisher";
    }
    void execute(const std::vector<std::string>& args) override {
        if (args.size() != 3) { err("Usage: register <username> <password> <type>"); return; }
        if (Session::instance().isLoggedIn()) { err("Already logged in. Logout first."); return; }

        const std::string& username = args[0];
        const std::string& password = args[1];
        const std::string& typeStr = args[2];

        if (!User::validateUsername(username)) {
            err("Username must be 6-24 characters."); return;
        }
        if (!User::validatePassword(password)) {
            err("Password must be 12-36 chars with at least one lowercase, one uppercase and one non-letter."); return;
        }
        UserType ut;
        if (typeStr == "reader")    ut = UserType::READER;
        else if (typeStr == "author")    ut = UserType::AUTHOR;
        else if (typeStr == "publisher") ut = UserType::PUBLISHER;
        else { err("Type must be reader, author, or publisher."); return; }

        auto& reg = UserRegistry::instance();
        if (!reg.registerUser(username, password, ut, Session::instance().today)) {
            err("Username already taken."); return;
        }
        ok("Registered as " + typeStr + ": " + username);
    }
};


class LoginCommand : public Command {
public:
    std::string name() const override { return "login"; }
    std::string helpText() const override { return "login <username> <password>"; }
    void execute(const std::vector<std::string>& args) override {
        if (args.size() != 2) { err("Usage: login <username> <password>"); return; }
        if (Session::instance().isLoggedIn()) { err("Already logged in."); return; }

        User* u = UserRegistry::instance().find(args[0]);
        if (!u || u->password != args[1]) { err("Invalid username or password."); return; }

        Session::instance().currentUser = u;
        ok("Logged in as " + u->username + " (" + u->typeString() + ")");
    }
};


class LogoutCommand : public Command {
public:
    std::string name() const override { return "logout"; }
    std::string helpText() const override { return "logout  — end current session"; }
    void execute(const std::vector<std::string>&) override {
        if (!Session::instance().isLoggedIn()) { err("Not logged in."); return; }
        std::string u = Session::instance().currentUser->username;
        Session::instance().logout();
        ok("Logged out: " + u);
    }
};


class SearchCommand : public Command {
public:
    std::string name() const override { return "search"; }
    std::string helpText() const override { return "search <name>  — fuzzy search users and books"; }
    void execute(const std::vector<std::string>& args) override {
        if (args.empty()) { err("Usage: search <name>"); return; }
        if (!Session::instance().isLoggedIn()) { err("Must be logged in."); return; }

        std::string query;
        for (size_t i = 0; i < args.size(); ++i) { if (i) query += " "; query += args[i]; }

        auto users = UserRegistry::instance().search(query);
        out("Users:");
        for (auto* u : users)
            out("  " + u->username + " (" + u->typeString() + ")");
        if (users.empty()) out("  (none)");

        auto books = BookDatabase::instance().search(query);
        out("Books:");
        for (auto* b : books) {
            std::ostringstream oss;
            oss << "  " << b->title << " (" << std::fixed << std::setprecision(2) << b->avgRating << ")";
            out(oss.str());
        }
        if (books.empty()) out("  (none)");
    }
};


class FollowCommand : public Command {
public:
    std::string name() const override { return "follow"; }
    std::string helpText() const override { return "follow <username>  — follow a user"; }
    void execute(const std::vector<std::string>& args) override {
        if (args.size() != 1) { err("Usage: follow <username>"); return; }
        if (!Session::instance().isLoggedIn()) { err("Must be logged in."); return; }

        User* me = Session::instance().currentUser;
        User* target = UserRegistry::instance().find(args[0]);
        if (!target) { err("User not found: " + args[0]); return; }
        if (target == me) { err("Cannot follow yourself."); return; }
        if (me->isFollowing(target->username)) { err("Already following " + target->username); return; }

        me->following.push_back(target->username);
        target->addFollower(me->username);

        
        Message msg(me->username,
            me->username + " started following you.",
            MessageType::FOLLOW_NOTIFICATION);
        if (target->type() == UserType::PUBLISHER)
            static_cast<Publisher*>(target)->receiveMessage(msg);
        else
            static_cast<Reader*>(target)->receiveMessage(msg);

        ok("Now following " + target->username);
    }
};

