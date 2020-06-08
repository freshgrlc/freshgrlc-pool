#ifndef MESSAGEBUS_MESSAGEFACTORY_H
#define MESSAGEBUS_MESSAGEFACTORY_H

#include "AbstractMessage.h"


class MessageFactory
{
    public:
        typedef AbstractMessage::Instantiator MessageInstantiator;

        MessageFactory &doRegisterMessageBusMessageImplementation(const std::string &messageClass, const std::string &messageType, const MessageInstantiator instantiator);

        static MessageRef makeMessage(std::string &&messageClass, std::string &&messageType, std::string &&content);

        inline static MessageFactory &get(void) { return *_instance; }

    private:
        class MessageImplementation
        {
            public:
                const std::string &messageClass;
                const std::string &messageType;
                const MessageInstantiator instantiator;

                inline MessageImplementation(const std::string &messageClass, const std::string &messageType, const MessageInstantiator instantiator) :
                    messageClass(messageClass),
                    messageType(messageType),
                    instantiator(instantiator)
                {}
        };

        static const std::unique_ptr<MessageFactory> _instance;
        static std::vector<MessageImplementation> implementations;

        inline MessageFactory(void) {}

        MessageRef doMakeMessage(std::string &&messageClass, std::string &&messageType, std::string &&content);

};

#define registerMessageBusMessageImplementation(T)  doRegisterMessageBusMessageImplementation(T::CLASS, T::TYPE, T::make)

#endif
