#include "Author.h"
#include <algorithm>

Author::Author(const std::string& u, const std::string& p, const Date& reg)
    : Reader(u, p, reg) {}

void Author::addPublishedBook(const std::string& title) {
    if (std::find(publishedBooks.begin(), publishedBooks.end(), title) == publishedBooks.end())
        publishedBooks.push_back(title);
}

void Author::addPublisher(const std::string& name) {
    if (!worksWithPublisher(name))
        publisherNames.push_back(name);
}

void Author::removePublisher(const std::string& name) {
    publisherNames.erase(
        std::remove(publisherNames.begin(), publisherNames.end(), name),
        publisherNames.end());
}

bool Author::worksWithPublisher(const std::string& name) const {
    return std::find(publisherNames.begin(), publisherNames.end(), name) != publisherNames.end();
}