#include "Date.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>
bool Date::isValid() const {
    if (year < 1 || month < 1 || month > 12 || day < 1) return false;
    int days_in_month[] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };
    bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (month == 2 && leap) days_in_month[2] = 29;
    return day <= days_in_month[month];
}

std::string Date::toString() const {
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << day << "."
        << std::setw(2) << std::setfill('0') << month << "."
        << year;
    return oss.str();
}

bool Date::parse(const std::string& s, Date& out) {    
    if (s.size() < 8) return false;
    try {
        char sep1 = s[2], sep2 = s[5];
        if ((sep1 != '.' && sep1 != '/') || (sep2 != '.' && sep2 != '/')) return false;
        int d = std::stoi(s.substr(0, 2));
        int m = std::stoi(s.substr(3, 2));
        int y = std::stoi(s.substr(6));
        Date tmp(d, m, y);
        if (!tmp.isValid()) return false;
        out = tmp;
        return true;
    }
    catch (...) {
        return false;
    }
}

bool Date::operator<(const Date& o) const {
    if (year != o.year) return year < o.year;
    if (month != o.month) return month < o.month;
    return day < o.day;
}