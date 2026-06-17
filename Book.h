#pragma once
#include <string>
#include <vector>
#include "Date.h"
class Book
{
public:
    std::string title;       
    std::string authorName;
    std::string publisherName;
    std::string synopsis;
    std::vector<std::string> genres;
    double avgRating = 0.0;
    int ratingCount = 0;
    Date releaseDate;
    int pageCount = 0;

    Book() = default;
    Book(const std::string& title,
        const std::string& author,
        const std::string& publisher,
        const Date& date,
        int pages)
        : title(title), authorName(author), publisherName(publisher),
        releaseDate(date), pageCount(pages) {}

    
    void addRating(double r);

    std::string genresString() const;
};

