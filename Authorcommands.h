#pragma once
#include "Command.h"
#include "Session.h"
#include "UserRegistry.h"
#include "Author.h"
#include "Publisher.h"
#include <algorithm>

static Author* requireAuthor(Command* cmd) {
    auto* u = Session::instance().currentUser;
    if (!u) return nullptr;
    return dynamic_cast<Author*>(u);
}


class ShowInboxAuthorCommand : public Command {
public:
    std::string name() const override { return "show-inbox"; }
    std::string helpText() const override { return "show-inbox [filter]  — filter: offers (shows only job offers)"; }
    void execute(const std::vector<std::string>& args) override {
        if (!Session::instance().isLoggedIn()) { err("Must be logged in."); return; }
        Author* a = requireAuthor(this);
        if (!a) { err("Only authors can use this version of show-inbox."); return; }

        bool onlyOffers = !args.empty() && args[0] == "offers";

        int idx = 0;
        for (auto& msg : a->inbox) {
            if (onlyOffers && msg.type != MessageType::JOB_OFFER) { ++idx; continue; }
            std::string readTag = msg.isRead ? "[read]" : "[unread]";
            std::string typeTag = "[" + msg.typeString() + "]";
            std::cout << "[" << idx << "] " << readTag << " " << typeTag
                << " From: " << msg.from << " | " << msg.content << "\n";
            ++idx;
        }
        if (a->inbox.empty()) std::cout << "Inbox is empty.\n";
    }
};


class AcceptOfferCommand : public Command {
public:
    std::string name() const override { return "accept-offer"; }
    std::string helpText() const override { return "accept-offer <index>  — accept a job offer"; }
    void execute(const std::vector<std::string>& args) override {
        if (!Session::instance().isLoggedIn()) { err("Must be logged in."); return; }
        Author* a = requireAuthor(this);
        if (!a) { err("Only authors can accept offers."); return; }
        if (args.size() != 1) { err("Usage: accept-offer <index>"); return; }

        int idx;
        try { idx = std::stoi(args[0]); }
        catch (...) { err("Invalid index."); return; }
        if (idx < 0 || idx >= (int)a->inbox.size()) { err("Index out of range."); return; }

        Message& msg = a->inbox[idx];
        if (msg.type != MessageType::JOB_OFFER) { err("Message is not a job offer."); return; }

        std::string publisherName = msg.from;
        msg.isRead = true;

        a->addPublisher(publisherName);

        Publisher* pub = UserRegistry::instance().findPublisher(publisherName);
        if (pub) pub->addAuthor(a->username);

        
        Message reply(a->username,
            a->username + " accepted your job offer.",
            MessageType::GENERAL);
        if (pub) pub->receiveMessage(reply);

        ok("Accepted offer from " + publisherName);
    }
};


class LeaveCommand : public Command {
public:
    std::string name() const override { return "leave"; }
    std::string helpText() const override { return "leave <publisherUsername>  — leave a publisher"; }
    void execute(const std::vector<std::string>& args) override {
        if (!Session::instance().isLoggedIn()) { err("Must be logged in."); return; }
        Author* a = requireAuthor(this);
        if (!a) { err("Only authors can leave publishers."); return; }
        if (args.size() != 1) { err("Usage: leave <publisherUsername>"); return; }

        const std::string& pubName = args[0];
        if (!a->worksWithPublisher(pubName)) { err("You don't work with: " + pubName); return; }

        a->removePublisher(pubName);
        Publisher* pub = UserRegistry::instance().findPublisher(pubName);
        if (pub) {
            auto& an = pub->authorNames;
            an.erase(std::remove_if(an.begin(), an.end(),
                [&](const std::string& s) { return s == a->username; }), an.end());
        }
        ok("Left publisher: " + pubName);
    }
};


class FollowersCommand : public Command {
public:
    std::string name() const override { return "followers"; }
    std::string helpText() const override { return "followers  — list your followers (author only)"; }
    void execute(const std::vector<std::string>&) override {
        if (!Session::instance().isLoggedIn()) { err("Must be logged in."); return; }
        Author* a = requireAuthor(this);
        if (!a) { err("Only authors can use this command."); return; }

        std::cout << "Followers of " << a->username << ":\n";
        for (auto& f : a->followers)
            std::cout << "  " << f << "\n";
        if (a->followers.empty()) std::cout << "  (none)\n";
    }
};

