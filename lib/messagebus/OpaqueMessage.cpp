#include <nlohmann/json.hpp>

#include "OpaqueMessage.h"

OpaqueMessage::OpaqueMessage(const std::string &messageClass, const std::string &messageType, const std::string &content) : AbstractMessage(messageClass, messageType),
    content(content)
{
}

OpaqueMessage::OpaqueMessage(std::string &&messageClass, std::string &&messageType, std::string &&content) : AbstractMessage(std::move(messageClass), std::move(messageType)),
    content(std::move(content))
{
}

OpaqueMessage::OpaqueMessage(const OpaqueMessage &them) : AbstractMessage(them.messageClass(), them.messageType()),
    content(them.content)
{
}

void OpaqueMessage::encodeContent() const
{
    this->setContent(json::parse(this->content));
}
