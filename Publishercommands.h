#pragma once
#include "Command.h"
#include "Session.h"
#include "UserRegistry.h"
#include "BookDatabase.h"
#include "Publisher.h"
#include "Author.h"
#include <sstream>
#include <iomanip>

static Publisher* requirePublisher(Command* cmd) {
    auto* u = Session::instance().currentUser;
    if (!u) return nullptr;
    return dynamic_cast<Publisher*>(u);
}


class PublishCommand : public Command {
public:
    std::string name() const override { return "publish"; }
    std::string helpText() const override {
        return "publish <bookTitle> <authorName> <releaseDate> <pageCount> <genre1> [genre2...]";
    }
    void execute(const std::vector<std::string>& args) override {
        if (!Session::instance().isLoggedIn()) { err("Must be logged in."); return; }
        Publisher* pub = requirePublisher(this);
        if (!pub) { err("Only publishers can publish books."); return; }
        if (args.size() < 5) { err("Usage: publish <title> <author> <date> <pages> <genres...>"); return; }

        
        std::string title = args[0];
        std::string authorName = args[1];
        std::string dateStr = args[2];
        int pages;
        try { pages = std::stoi(args[3]); }
        catch (...) { err("Invalid page count."); return; }

        Date releaseDate;
        if (!Date::parse(dateStr, releaseDate)) { err("Invalid date. Use DD.MM.YYYY"); return; }

        if (BookDatabase::instance().exists(title)) { err("Book already exists: " + title); return; }

        Book book(title, authorName, pub->username, releaseDate, pages);
        for (size_t i = 4; i < args.size(); ++i) book.genres.push_back(args[i]);

        BookDatabase::instance().addBook(book);
        pub->addBook(title);
        pub->addAuthor(authorName);

        
        Author* author = UserRegistry::instance().findAuthor(authorName);
        if (author) {
            author->addPublishedBook(title);
            author->addPublisher(pub->username);
        }

        
        auto& reg = UserRegistry::instance();
        for (auto& [uname, uptr] : reg.all()) {
            User* u = uptr.get();
            if (u == pub) continue;

            bool followsAuthor = author && u->isFollowing(authorName);
            bool followsPublisher = u->isFollowing(pub->username);
            if (!followsAuthor && !followsPublisher) continue;

            std::string content;
            if (followsAuthor && followsPublisher)
                content = "New book published by author " + authorName +
                " and publisher " + pub->username + ": " + title;
            else if (followsAuthor)
                content = "New book published by author " + authorName + ": " + title;
            else
                content = "New book published by publisher " + pub->username + ": " + title;

            Message msg(pub->username, content, MessageType::BOOK_NOTIFICATION);
            if (u->type() == UserType::PUBLISHER)
                static_cast<Publisher*>(u)->receiveMessage(msg);
            else
                static_cast<Reader*>(u)->receiveMessage(msg);
        }

        ok("Published \"" + title + "\" by " + authorName);
    }
};


class AddSynopsisCommand : public Command {
public:
    std::string name() const override { return "add-synopsis"; }
    std::string helpText() const override { return "add-synopsis <bookTitle> <synopsis>"; }
    void execute(const std::vector<std::string>& args) override {
        if (!Session::instance().isLoggedIn()) { err("Must be logged in."); return; }
        Publisher* pub = requirePublisher(this);
        if (!pub) { err("Only publishers can add synopses."); return; }
        if (args.size() < 2) { err("Usage: add-synopsis <bookTitle> <synopsis>"); return; }

        std::string title = args[0];
        std::string synopsis;
        for (size_t i = 1; i < args.size(); ++i) { if (i > 1) synopsis += " "; synopsis += args[i]; }

        Book* book = BookDatabase::instance().find(title);
        if (!book) { err("Book not found: " + title); return; }
        if (book->publisherName != pub->username) { err("You are not the publisher of this book."); return; }

        book->synopsis = synopsis;
        ok("Synopsis updated for \"" + title + "\"");
    }
};


class OfferCommand : public Command {
public:
    std::string name() const override { return "offer"; }
    std::string helpText() const override { return "offer <authorUsername>  — send a job offer"; }
    void execute(const std::vector<std::string>& args) override {
        if (!Session::instance().isLoggedIn()) { err("Must be logged in."); return; }
        Publisher* pub = requirePublisher(this);
        if (!pub) { err("Only publishers can send job offers."); return; }
        if (args.size() != 1) { err("Usage: offer <authorUsername>"); return; }

        Author* author = UserRegistry::instance().findAuthor(args[0]);
        if (!author) { err("Author not found: " + args[0]); return; }

        Message msg(pub->username,
            "Job offer from publisher " + pub->username + ". Use accept-offer <index> to accept.",
            MessageType::JOB_OFFER);
        author->receiveMessage(msg);
        ok("Job offer sent to " + author->username);
    }
};

