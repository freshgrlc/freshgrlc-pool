#include <nlohmann/json.hpp>

#include "AbstractMessage.h"
#include "OpaqueMessage.h"
#include "MessageFactory.h"


AbstractMessage::AbstractMessage(const std::string &messageClass, const std::string &messageType) :
    _messageClass(messageClass),
    _messageType(messageType)
{
}

AbstractMessage::AbstractMessage(std::string &&messageClass, std::string &&messageType) :
    _messageClass(std::move(messageClass)),
    _messageType(std::move(messageType))
{
}

AbstractMessage::~AbstractMessage()
{
}

void AbstractMessage::setContent(const json &content) const
{
    this->_content = std::make_unique<json>(content);
}

void AbstractMessage::setContent(json &&content) const
{
    this->_content = std::make_unique<json>(std::move(content));
}

std::unique_ptr<AbstractMessage> AbstractMessage::decode(const ConstByteStringRef &message)
{
    return doDecode(message, MessageFactory::makeMessage);
}

std::unique_ptr<AbstractMessage> AbstractMessage::decodeOpaque(const ConstByteStringRef &message)
{
    return doDecode(message, OpaqueMessage::make);
}

std::unique_ptr<AbstractMessage> AbstractMessage::doDecode(const ConstByteStringRef &message, Instantiator instantiator)
{
    if (message.length > MAX_MESSAGE_SIZE)
        throw std::runtime_error("Message of size " + std::to_string(message.length) + " too large!");

    auto reader = message.reader();

    size_t messageSize = (size_t) reader.read<uint32_t>();

    if (messageSize != reader.remaining())
        throw std::runtime_error("Message of size " + std::to_string(message.length) + " does not match header size of " + std::to_string(messageSize));

    std::string messageClass;
    std::string messageType;
    std::string content;

    reader  >> messageClass
            >> messageType
            >> content;

    if (reader.remaining())
        throw std::runtime_error("Message of size " + std::to_string(message.length) + " does not match content size (extra " + std::to_string(reader.remaining()) + " bytes remaining");

    return instantiator(std::move(messageClass), std::move(messageType), std::move(content));
}

ByteString AbstractMessage::encode() const
{
    this->encodeContent();

    ByteString payload;

    payload << this->messageClass()
            << this->messageType()
            << _content->dump();

    return ByteString() << payload;
}

std::string AbstractMessage::raw() const
{
    if (!_content)
        this->encodeContent();

    return _content->dump();
}
