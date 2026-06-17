#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include "Command.h"
#include "BasicCommands.h"
#include "ReaderCommands.h"
#include "PublisherCommands.h"
#include "AuthorCommands.h"
#include "Session.h"
#include "FileSerializer.h"

class CommandDispatcher {
public:
    CommandDispatcher() {
        
        add<RegisterCommand>();
        add<LoginCommand>();
        add<LogoutCommand>();
        add<SearchCommand>();
        add<FollowCommand>();

        
        add<AddBookCommand>();
        add<DeleteBookCommand>();
        add<CreateShelfCommand>();
        add<DeleteShelfCommand>();
        add<AddToShelfCommand>();
        add<RemoveFromShelfCommand>();
        add<ShowShelfCommand>();
        add<ReadMsgCommand>();
        add<DeleteMsgCommand>();
        add<FriendsCommand>();
        add<AddBirthdayCommand>();
        add<ProfileCommand>();

        
        add<PublishCommand>();
        add<AddSynopsisCommand>();
        add<OfferCommand>();

        
        add<AcceptOfferCommand>();
        add<LeaveCommand>();
        add<FollowersCommand>();
    }

    
    bool dispatch(const std::string& line, const std::string& dataFile) {
        auto [cmd, args] = parse(line);
        if (cmd.empty()) return true;

        if (cmd == "exit") {
            FileSerializer::save(dataFile);
            std::cout << "[OK] Data saved. Goodbye.\n";
            return false;
        }

        if (cmd == "help") {
            showHelp();
            return true;
        }

        
        if (cmd == "show-inbox") {
            User* u = Session::instance().currentUser;
            if (!u) { std::cout << "[ERROR] Must be logged in.\n"; return true; }
            if (u->type() == UserType::AUTHOR) {
                ShowInboxAuthorCommand c;
                c.execute(args);
            }
            else if (u->type() == UserType::READER) {
                ShowInboxReaderCommand c;
                c.execute(args);
            }
            else {
                std::cout << "[ERROR] Publishers don't have an inbox accessible this way.\n";
            }
            return true;
        }

        auto it = commands_.find(cmd);
        if (it == commands_.end()) {
            std::cout << "[ERROR] Unknown command: " << cmd << ". Type 'help' for a list.\n";
            return true;
        }
        it->second->execute(args);
        return true;
    }

private:
    std::map<std::string, std::unique_ptr<Command>> commands_;

    template<typename T>
    void add() {
        auto c = std::make_unique<T>();
        std::string n = c->name();
        commands_[n] = std::move(c);
    }

    std::pair<std::string, std::vector<std::string>> parse(const std::string& line) {
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> tokens;
        while (iss >> token) tokens.push_back(token);
        if (tokens.empty()) return { "", {} };
        std::string cmd = tokens[0];
        tokens.erase(tokens.begin());
        return { cmd, tokens };
    }

    void showHelp() {
        User* u = Session::instance().currentUser;
        std::cout << "=== Available commands ===\n";
        std::cout << "  help\n";
        std::cout << "  register <username> <password> <type>\n";
        std::cout << "  login <username> <password>\n";
        std::cout << "  logout\n";
        std::cout << "  exit\n";

        if (!u) return;

        std::cout << "  search <name>\n";
        std::cout << "  follow <username>\n";

        if (u->type() == UserType::READER || u->type() == UserType::AUTHOR) {
            std::cout << "  add-book <bookName> <status> [rating]\n";
            std::cout << "  delete-book <bookName>\n";
            std::cout << "  create-shelf <name>\n";
            std::cout << "  delete-shelf <name>\n";
            std::cout << "  add-to-shelf <bookName> <shelfName>\n";
            std::cout << "  remove-from-shelf <bookName> <shelfName>\n";
            std::cout << "  show-shelf [reader] <name>\n";
            std::cout << "  show-inbox [friends]\n";
            std::cout << "  read-msg <index>\n";
            std::cout << "  delete-msg <index>\n";
            std::cout << "  friends [reader]\n";
            std::cout << "  add-birthday [date]\n";
            std::cout << "  profile [reader]\n";
        }
        if (u->type() == UserType::AUTHOR) {
            std::cout << "  show-inbox [offers]\n";
            std::cout << "  accept-offer <index>\n";
            std::cout << "  leave <publisher>\n";
            std::cout << "  followers\n";
        }
        if (u->type() == UserType::PUBLISHER) {
            std::cout << "  publish <title> <author> <date> <pages> <genres...>\n";
            std::cout << "  add-synopsis <title> <synopsis>\n";
            std::cout << "  offer <author>\n";
        }
    }
};

