#pragma once
#include <string>
class Date
{
public:
    int day = 0, month = 0, year = 0;

    Date() = default;
    Date(int d, int m, int y) : day(d), month(m), year(y) {}

    bool isValid() const;
    std::string toString() const;
    
    static bool parse(const std::string& s, Date& out);

    bool operator==(const Date& o) const { return day == o.day && month == o.month && year == o.year; }
    bool operator<(const Date& o) const;
};

