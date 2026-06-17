#include "Book.h"
#include <sstream>

void Book::addRating(double r) {
    double total = avgRating * ratingCount + r;
    ratingCount++;
    avgRating = total / ratingCount;
}

std::string Book::genresString() const {
    std::ostringstream oss;
    for (size_t i = 0; i < genres.size(); ++i) {
        if (i) oss << ", ";
        oss << genres[i];
    }
    return oss.str();
}