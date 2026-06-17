#include "Message.h"
std::string Message::typeString() const {
    switch (type) {
    case MessageType::FOLLOW_NOTIFICATION: return "FOLLOW";
    case MessageType::JOB_OFFER:           return "JOB_OFFER";
    case MessageType::BOOK_NOTIFICATION:   return "BOOK_NOTIFICATION";
    default:                               return "GENERAL";
    }
}

MessageType Message::typeFromString(const std::string& s) {
    if (s == "FOLLOW")            return MessageType::FOLLOW_NOTIFICATION;
    if (s == "JOB_OFFER")         return MessageType::JOB_OFFER;
    if (s == "BOOK_NOTIFICATION") return MessageType::BOOK_NOTIFICATION;
    return MessageType::GENERAL;
}