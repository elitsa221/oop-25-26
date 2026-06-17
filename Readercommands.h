#pragma once
#include "Command.h"
#include "Session.h"
#include "UserRegistry.h"
#include "BookDatabase.h"
#include "Reader.h"
#include <sstream>
#include <iomanip>
#include <algorithm>


static Reader* requireReader(Command* cmd) {
    auto* u = Session::instance().currentUser;
    if (!u) return nullptr;
    auto* r = dynamic_cast<Reader*>(u);
    return r;
}


class AddBookCommand : public Command {
public:
    std::string name() const override { return "add-book"; }
    std::string helpText() const override {
        return "add-book <bookName> <status> [rating]  — status: plan-to-read/reading/paused/dropped";
    }
    void execute(const std::vector<std::string>& args) override {
        if (!Session::instance().isLoggedIn()) { err("Must be logged in."); return; }
        Reader* r = requireReader(this);
        if (!r) { err("Only readers and authors can add books."); return; }
        if (args.size() < 2) { err("Usage: add-book <bookName> <status> [rating]"); return; }

        
        std::string statusStr, ratingStr;
        size_t statusIdx;
        double rating = -1.0;

        if (args.size() >= 3) {
            
            try {
                rating = std::stod(args.back());
                statusIdx = args.size() - 2;
            }
            catch (...) {
                statusIdx = args.size() - 1;
            }
        }
        else {
            statusIdx = args.size() - 1;
        }

        statusStr = args[statusIdx];
        std::string title;
        for (size_t i = 0; i < statusIdx; ++i) { if (i) title += " "; title += args[i]; }

        if (!BookDatabase::instance().exists(title)) {
            err("Book not found: " + title); return;
        }

        ReadStatus status;
        try { status = statusFromString(statusStr); }
        catch (...) { err("Invalid status: " + statusStr); return; }

        if (!r->addBook(title, status, rating)) {
            err("Book already in your profile."); return;
        }

        
        if (rating >= 0) {
            Book* b = BookDatabase::instance().find(title);
            if (b) b->addRating(rating);
        }
        ok("Added \"" + title + "\" with status: " + statusStr);
    }
};

class DeleteBookCommand : public Command {
public:
    std::string name() const override { return "delete-book"; }
    std::string helpText() const override { return "delete-book <bookName>  — remove book from profile and all shelves"; }
    void execute(const std::vector<std::string>& args) override {
        if (!Session::instance().isLoggedIn()) { err("Must be logged in."); return; }
        Reader* r = requireReader(this);
        if (!r) { err("Only readers and authors can use this."); return; }
        if (args.empty()) { err("Usage: delete-book <bookName>"); return; }

        std::string title;
        for (size_t i = 0; i < args.size(); ++i) { if (i) title += " "; title += args[i]; }

        if (!r->removeBook(title)) { err("Book not in your profile: " + title); return; }
        ok("Removed \"" + title + "\" from your profile.");
    }
};

class CreateShelfCommand : public Command {
public:
    std::string name() const override { return "create-shelf"; }
    std::string helpText() const override { return "create-shelf <name>"; }
    void execute(const std::vector<std::string>& args) override {
        if (!Session::instance().isLoggedIn()) { err("Must be logged in."); return; }
        Reader* r = requireReader(this);
        if (!r) { err("Only readers and authors can use shelves."); return; }
        if (args.empty()) { err("Usage: create-shelf <name>"); return; }

        std::string name;
        for (size_t i = 0; i < args.size(); ++i) { if (i) name += " "; name += args[i]; }

        if (!r->createShelf(name, Session::instance().today)) {
            err("Shelf already exists: " + name); return;
        }
        ok("Shelf created: " + name);
    }
};

class DeleteShelfCommand : public Command {
public:
    std::string name() const override { return "delete-shelf"; }
    std::string helpText() const override { return "delete-shelf <name>"; }
    void execute(const std::vector<std::string>& args) override {
        if (!Session::instance().isLoggedIn()) { err("Must be logged in."); return; }
        Reader* r = requireReader(this);
        if (!r) { err("Only readers and authors can use shelves."); return; }
        if (args.empty()) { err("Usage: delete-shelf <name>"); return; }

        std::string name;
        for (size_t i = 0; i < args.size(); ++i) { if (i) name += " "; name += args[i]; }

        if (!r->deleteShelf(name)) { err("Shelf not found: " + name); return; }
        ok("Shelf deleted: " + name);
    }
};

class AddToShelfCommand : public Command {
public:
    std::string name() const override { return "add-to-shelf"; }
    std::string helpText() const override { return "add-to-shelf <bookName> <shelfName>"; }
    void execute(const std::vector<std::string>& args) override {
        if (!Session::instance().isLoggedIn()) { err("Must be logged in."); return; }
        Reader* r = requireReader(this);
        if (!r) { err("Only readers and authors can use shelves."); return; }
        if (args.size() < 2) { err("Usage: add-to-shelf <bookName> <shelfName>"); return; }
       
        std::string shelfName = args.back();
        std::string title;
        for (size_t i = 0; i + 1 < args.size(); ++i) { if (i) title += " "; title += args[i]; }

        if (!r->hasBook(title)) { err("Book not in your profile: " + title); return; }
        Shelf* shelf = r->findShelf(shelfName);
        if (!shelf) { err("Shelf not found: " + shelfName); return; }
        if (!shelf->addBook(title)) { err("Book already on shelf."); return; }
        ok("Added \"" + title + "\" to shelf \"" + shelfName + "\"");
    }
};

class RemoveFromShelfCommand : public Command {
public:
    std::string name() const override { return "remove-from-shelf"; }
    std::string helpText() const override { return "remove-from-shelf <bookName> <shelfName>"; }
    void execute(const std::vector<std::string>& args) override {
        if (!Session::instance().isLoggedIn()) { err("Must be logged in."); return; }
        Reader* r = requireReader(this);
        if (!r) { err("Only readers and authors can use shelves."); return; }
        if (args.size() < 2) { err("Usage: remove-from-shelf <bookName> <shelfName>"); return; }

        std::string shelfName = args.back();
        std::string title;
        for (size_t i = 0; i + 1 < args.size(); ++i) { if (i) title += " "; title += args[i]; }

        Shelf* shelf = r->findShelf(shelfName);
        if (!shelf) { err("Shelf not found: " + shelfName); return; }
        if (!shelf->removeBook(title)) { err("Book not on that shelf."); return; }
        ok("Removed \"" + title + "\" from shelf \"" + shelfName + "\"");
    }
};

class ShowShelfCommand : public Command {
public:
    std::string name() const override { return "show-shelf"; }
    std::string helpText() const override {
        return "show-shelf [reader] <shelfName>  — show books on a shelf (reader optional; must be friends)";
    }
    void execute(const std::vector<std::string>& args) override {
        if (!Session::instance().isLoggedIn()) { err("Must be logged in."); return; }
        Reader* me = requireReader(this);
        if (!me) { err("Only readers and authors can use shelves."); return; }
        if (args.empty()) { err("Usage: show-shelf [reader] <name>"); return; }

        Reader* target = me;
        std::string shelfName;
        
        if (args.size() >= 2) {
            User* u = UserRegistry::instance().find(args[0]);
            if (u && u != me) {
                Reader* r = dynamic_cast<Reader*>(u);
                if (!r) { err("That user is not a reader."); return; }
                if (!me->isFriendWith(u)) { err("Access denied: not friends with " + u->username); return; }
                target = r;                
                for (size_t i = 1; i < args.size(); ++i) { if (i > 1) shelfName += " "; shelfName += args[i]; }
            }
            else {
                for (size_t i = 0; i < args.size(); ++i) { if (i) shelfName += " "; shelfName += args[i]; }
            }
        }
        else {
            shelfName = args[0];
        }

        Shelf* shelf = target->findShelf(shelfName);
        if (!shelf) { err("Shelf not found: " + shelfName); return; }

        out("Shelf: " + shelf->name + " (" + std::to_string(shelf->bookCount()) + " books)");
        for (auto& title : shelf->bookTitles) {
            const Book* b = BookDatabase::instance().find(title);
            std::ostringstream oss;
            oss << "  " << title;
            if (b) oss << " (" << std::fixed << std::setprecision(2) << b->avgRating << ")";
            out(oss.str());
        }
    }
};
class ShowInboxReaderCommand : public Command {
public:
    std::string name() const override { return "show-inbox"; }
    std::string helpText() const override { return "show-inbox [filter]  — filter: friends (shows only follow notifications)"; }
    void execute(const std::vector<std::string>& args) override {
        if (!Session::instance().isLoggedIn()) { err("Must be logged in."); return; }
        Reader* r = requireReader(this);
        if (!r) { err("Only readers and authors can use this."); return; }

        bool onlyFollow = !args.empty() && args[0] == "friends";

        int idx = 0;
        for (auto& msg : r->inbox) {
            if (onlyFollow && msg.type != MessageType::FOLLOW_NOTIFICATION) { ++idx; continue; }
            std::string readTag = msg.isRead ? "[read]" : "[unread]";
            out("[" + std::to_string(idx) + "] " + readTag + " From: " + msg.from + " | " + msg.content);
            ++idx;
        }
        if (r->inbox.empty()) out("Inbox is empty.");
    }
};

class ReadMsgCommand : public Command {
public:
    std::string name() const override { return "read-msg"; }
    std::string helpText() const override { return "read-msg <index>  — mark message as read"; }
    void execute(const std::vector<std::string>& args) override {
        if (!Session::instance().isLoggedIn()) { err("Must be logged in."); return; }
        Reader* r = requireReader(this);
        if (!r) { err("Only readers and authors can use this."); return; }
        if (args.size() != 1) { err("Usage: read-msg <index>"); return; }

        int idx;
        try { idx = std::stoi(args[0]); }
        catch (...) { err("Invalid index."); return; }
        if (idx < 0 || idx >= (int)r->inbox.size()) { err("Index out of range."); return; }

        r->inbox[idx].isRead = true;
        out("Message " + std::to_string(idx) + " marked as read.");
    }
};

class DeleteMsgCommand : public Command {
public:
    std::string name() const override { return "delete-msg"; }
    std::string helpText() const override { return "delete-msg <index>  — delete a read message"; }
    void execute(const std::vector<std::string>& args) override {
        if (!Session::instance().isLoggedIn()) { err("Must be logged in."); return; }
        Reader* r = requireReader(this);
        if (!r) { err("Only readers and authors can use this."); return; }
        if (args.size() != 1) { err("Usage: delete-msg <index>"); return; }

        int idx;
        try { idx = std::stoi(args[0]); }
        catch (...) { err("Invalid index."); return; }
        if (idx < 0 || idx >= (int)r->inbox.size()) { err("Index out of range."); return; }
        if (!r->inbox[idx].isRead) { err("Message must be read before deleting."); return; }

        r->inbox.erase(r->inbox.begin() + idx);
        ok("Message deleted.");
    }
};

class FriendsCommand : public Command {
public:
    std::string name() const override { return "friends"; }
    std::string helpText() const override { return "friends [reader]  — list mutual followers"; }
    void execute(const std::vector<std::string>& args) override {
        if (!Session::instance().isLoggedIn()) { err("Must be logged in."); return; }
        Reader* me = requireReader(this);
        if (!me) { err("Only readers and authors can use this."); return; }

        Reader* target = me;
        if (!args.empty()) {
            User* u = UserRegistry::instance().find(args[0]);
            if (!u) { err("User not found: " + args[0]); return; }
            target = dynamic_cast<Reader*>(u);
            if (!target) { err("That user is not a reader."); return; }
        }

        out("Friends of " + target->username + ":");
        bool found = false;
        for (auto& fname : target->following) {
            User* other = UserRegistry::instance().find(fname);
            if (other && other->isFollowedBy(target->username)) {
                out("  " + fname);
                found = true;
            }
        }
        if (!found) out("  (none)");
    }
};

class AddBirthdayCommand : public Command {
public:
    std::string name() const override { return "add-birthday"; }
    std::string helpText() const override { return "add-birthday [date]  — set birthday (DD.MM.YYYY); omit to clear"; }
    void execute(const std::vector<std::string>& args) override {
        if (!Session::instance().isLoggedIn()) { err("Must be logged in."); return; }
        Reader* r = requireReader(this);
        if (!r) { err("Only readers and authors can set birthday."); return; }

        if (args.empty()) {
            r->hasBirthday = false;
            ok("Birthday cleared.");
            return;
        }

        Date d;
        if (!Date::parse(args[0], d)) { err("Invalid date. Use DD.MM.YYYY"); return; }
        r->birthday = d;
        r->hasBirthday = true;
        ok("Birthday set to " + d.toString());
    }
};
class ProfileCommand : public Command {
public:
    std::string name() const override { return "profile"; }
    std::string helpText() const override { return "profile [reader]  — show profile info"; }
    void execute(const std::vector<std::string>& args) override {
        if (!Session::instance().isLoggedIn()) { err("Must be logged in."); return; }
        Reader* me = requireReader(this);

        Reader* target = me;
        if (!args.empty()) {
            User* u = UserRegistry::instance().find(args[0]);
            if (!u) { err("User not found: " + args[0]); return; }
            target = dynamic_cast<Reader*>(u);
            if (!target) { err("That user is not a reader or author."); return; }
        }

        out("=== Profile: " + target->username + " (" + target->typeString() + ") ===");
        out("Registered: " + target->registrationDate.toString());
        out("Birthday:   " + (target->hasBirthday ? target->birthday.toString() : "(not set)"));
        out("Followers:  " + std::to_string(target->followers.size()));

        out("Shelves:");
        for (auto& s : target->shelves)
            out("  " + s.name + " (" + std::to_string(s.bookCount()) + " books)");

        out("Favorite books:");
        for (auto& t : target->favoriteBooks) out("  " + t);
        if (target->favoriteBooks.empty()) out("  (none)");
    }
};

