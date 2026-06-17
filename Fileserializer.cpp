#include "Fileserializer.h"
#include "BookDatabase.h"
#include "UserRegistry.h"
#include "Reader.h"
#include "Author.h"
#include "Publisher.h"
#include <fstream>
#include <sstream>
#include <stdexcept>


static std::string joinList(const std::vector<std::string>& v, char sep) {
    std::string r;
    for (size_t i = 0; i < v.size(); ++i) { if (i) r += sep; r += v[i]; }
    return r;
}

static std::vector<std::string> splitList(const std::string& s, char sep) {
    std::vector<std::string> r;
    if (s.empty()) return r;
    std::stringstream ss(s);
    std::string tok;
    while (std::getline(ss, tok, sep)) r.push_back(tok);
    return r;
}

static std::string dateStr(const Date& d) {
    return d.toString();
}

static Date parseDate(const std::string& s) {
    Date d;
    Date::parse(s, d);
    return d;
}

static std::string encode(const std::string& s) {
    std::string r;
    for (char c : s) {
        if (c == '|') r += "\\p";
        else if (c == '\n') r += "\\n";
        else if (c == '\\') r += "\\\\";
        else r += c;
    }
    return r;
}

static std::string decode(const std::string& s) {
    std::string r;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            char next = s[i + 1];
            if (next == 'p') { r += '|'; ++i; }
            else if (next == 'n') { r += '\n'; ++i; }
            else if (next == '\\') { r += '\\'; ++i; }
            else r += s[i];
        }
        else r += s[i];
    }
    return r;
}


void FileSerializer::save(const std::string& path) {
    std::ofstream f(path);
    if (!f) throw std::runtime_error("Cannot open file for writing: " + path);

    const char S = SEP;
    const char L = LIST_SEP;
    
    f << "[BOOKS]\n";
    for (auto& [title, book] : BookDatabase::instance().all()) {
        f << encode(book.title) << S
            << encode(book.authorName) << S
            << encode(book.publisherName) << S
            << encode(book.synopsis) << S
            << encode(joinList(book.genres, L)) << S
            << book.avgRating << S
            << book.ratingCount << S
            << dateStr(book.releaseDate) << S
            << book.pageCount << "\n";
    }
    
    f << "[USERS]\n";
    for (auto& [name, uptr] : UserRegistry::instance().all()) {
        User* u = uptr.get();        
        f << u->typeString() << S
            << encode(u->username) << S
            << encode(u->password) << S
            << dateStr(u->registrationDate) << S
            << encode(joinList(u->followers, L)) << S
            << encode(joinList(u->following, L));

        if (u->type() == UserType::PUBLISHER) {
            Publisher* p = static_cast<Publisher*>(u);
            f << S << encode(joinList(p->authorNames, L))
                << S << encode(joinList(p->publishedBooks, L));            
            f << S << (int)p->inbox.size();
            for (auto& msg : p->inbox) {
                f << S << encode(msg.from)
                    << S << encode(msg.content)
                    << S << msg.typeString()
                    << S << (msg.isRead ? 1 : 0);
            }
        }
        else {
            
            Reader* r = static_cast<Reader*>(u);
            
            f << S << (int)r->books.size();
            for (auto& be : r->books) {
                f << S << encode(be.title)
                    << S << statusToString(be.status)
                    << S << be.rating;
            }
            
            f << S << (int)r->shelves.size();
            for (auto& shelf : r->shelves) {
                f << S << encode(shelf.name)
                    << S << dateStr(shelf.createdDate)
                    << S << encode(joinList(shelf.bookTitles, L));
            }
            
            f << S << encode(joinList(r->favoriteBooks, L));
            
            f << S << (r->hasBirthday ? dateStr(r->birthday) : "");
            
            f << S << (int)r->inbox.size();
            for (auto& msg : r->inbox) {
                f << S << encode(msg.from)
                    << S << encode(msg.content)
                    << S << msg.typeString()
                    << S << (msg.isRead ? 1 : 0);
            }

            if (u->type() == UserType::AUTHOR) {
                Author* a = static_cast<Author*>(u);
                f << S << encode(joinList(a->publishedBooks, L))
                    << S << encode(joinList(a->publisherNames, L));
            }
        }
        f << "\n";
    }
    f << "[END]\n";
}



static std::vector<std::string> splitLine(const std::string& line, char sep) {
    std::vector<std::string> parts;
    std::string cur;
    for (char c : line) {
        if (c == sep) { parts.push_back(cur); cur.clear(); }
        else cur += c;
    }
    parts.push_back(cur);
    return parts;
}

void FileSerializer::load(const std::string& path) {
    std::ifstream f(path);
    if (!f) return;  

    const char S = SEP;
    const char L = LIST_SEP;

    std::string section, line;
    while (std::getline(f, line)) {
        if (line == "[BOOKS]" || line == "[USERS]" || line == "[END]") {
            section = line; continue;
        }
        if (line.empty()) continue;

        auto parts = splitLine(line, S);
        auto get = [&](size_t i) -> std::string {
            return i < parts.size() ? decode(parts[i]) : "";
        };

        if (section == "[BOOKS]") {
            Book book;
            book.title = get(0);
            book.authorName = get(1);
            book.publisherName = get(2);
            book.synopsis = get(3);
            book.genres = splitList(get(4), L);
            book.avgRating = std::stod(parts[5]);
            book.ratingCount = std::stoi(parts[6]);
            book.releaseDate = parseDate(get(7));
            book.pageCount = std::stoi(parts[8]);
            BookDatabase::instance().addBook(book);

        }
        else if (section == "[USERS]") {
            std::string typeStr = get(0);
            std::string username = get(1);
            std::string password = get(2);
            Date regDate = parseDate(get(3));
            auto followersVec = splitList(get(4), L);
            auto followingVec = splitList(get(5), L);

            UserType ut;
            if (typeStr == "Publisher")  ut = UserType::PUBLISHER;
            else if (typeStr == "Author") ut = UserType::AUTHOR;
            else                          ut = UserType::READER;

            UserRegistry::instance().registerUser(username, password, ut, regDate);
            User* u = UserRegistry::instance().find(username);
            if (!u) continue;
            u->followers = followersVec;
            u->following = followingVec;

            size_t idx = 6;

            auto readMessages = [&](std::vector<Message>& inbox) {
                int cnt = std::stoi(parts[idx++]);
                for (int i = 0; i < cnt; ++i) {
                    Message msg;
                    msg.from = get(idx++);
                    msg.content = get(idx++);
                    msg.type = Message::typeFromString(get(idx++));
                    msg.isRead = std::stoi(parts[idx++]) != 0;
                    inbox.push_back(msg);
                }
            };

            if (ut == UserType::PUBLISHER) {
                Publisher* p = static_cast<Publisher*>(u);
                p->authorNames = splitList(get(idx++), L);
                p->publishedBooks = splitList(get(idx++), L);
                readMessages(p->inbox);
            }
            else {
                Reader* r = static_cast<Reader*>(u);
                int bookCnt = std::stoi(parts[idx++]);
                for (int i = 0; i < bookCnt; ++i) {
                    BookEntry be;
                    be.title = get(idx++);
                    be.status = statusFromString(get(idx++));
                    be.rating = std::stod(parts[idx++]);
                    r->books.push_back(be);
                }
                int shelfCnt = std::stoi(parts[idx++]);
                for (int i = 0; i < shelfCnt; ++i) {
                    Shelf shelf;
                    shelf.name = get(idx++);
                    shelf.createdDate = parseDate(get(idx++));
                    shelf.bookTitles = splitList(get(idx++), L);
                    r->shelves.push_back(shelf);
                }
                r->favoriteBooks = splitList(get(idx++), L);
                std::string bday = get(idx++);
                if (!bday.empty()) {
                    Date::parse(bday, r->birthday);
                    r->hasBirthday = true;
                }
                readMessages(r->inbox);

                if (ut == UserType::AUTHOR) {
                    Author* a = static_cast<Author*>(u);
                    a->publishedBooks = splitList(get(idx++), L);
                    a->publisherNames = splitList(get(idx++), L);
                }
            }
        }
    }
}