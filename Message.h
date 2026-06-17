#pragma once
#include <string>
enum class MessageType {
    GENERAL,
    FOLLOW_NOTIFICATION,
    JOB_OFFER,
    BOOK_NOTIFICATION
};
class Message
{
public:
    std::string from;
    std::string content;
    MessageType type = MessageType::GENERAL;
    bool isRead = false;

    Message() = default;
    Message(const std::string& from, const std::string& content, MessageType type = MessageType::GENERAL)
        : from(from), content(content), type(type) {}

    std::string typeString() const;
    static MessageType typeFromString(const std::string& s);
};

