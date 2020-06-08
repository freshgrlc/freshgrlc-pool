#include <nlohmann/json.hpp>

#include "MessageFactory.h"


const std::unique_ptr<MessageFactory> MessageFactory::_instance = std::unique_ptr<MessageFactory>(new MessageFactory());
std::vector<MessageFactory::MessageImplementation> MessageFactory::implementations;


MessageFactory &MessageFactory::doRegisterMessageBusMessageImplementation(const std::string &messageClass, const std::string &messageType, const MessageFactory::MessageInstantiator instantiator)
{
    for (auto &implementation : this->implementations)
    {
        if (implementation.messageClass == messageClass && implementation.messageType == messageType)
            return *this;
    }

    this->implementations.emplace_back(messageClass, messageType, instantiator);
    return *this;
}

MessageRef MessageFactory::makeMessage(std::string &&messageClass, std::string &&messageType, std::string &&content)
{
    return _instance->doMakeMessage(std::move(messageClass), std::move(messageType), std::move(content));
}

MessageRef MessageFactory::doMakeMessage(std::string &&messageClass, std::string &&messageType, std::string &&content)
{
    for (auto &implementation : this->implementations)
    {
        if (implementation.messageClass == messageClass && implementation.messageType == messageType)
            return implementation.instantiator(std::move(messageClass), std::move(messageType), std::move(content));
    }

    throw std::runtime_error("Unsupported message bus message: '" + messageClass + "::" + messageType + "'!");
}
