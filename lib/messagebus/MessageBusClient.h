#ifndef MESSAGEBUS_MESSAGEBUSCLIENT_H
#define MESSAGEBUS_MESSAGEBUSCLIENT_H

#include <socket/ReconnectingClient.h>
#include <util/ByteString.h>

class AbstractMessage;

class MessageBusClient : private ReconnectingClient
{
    public:
        static const int DEFAULT_PORT;

        MessageBusClient(const std::string &host, int port = DEFAULT_PORT);

        void subscribe(const std::string &messageClass);
        void unsubscribe(const std::string &messageClass);

        void setSubscriptions(std::vector<std::string> &&messageClasses);

        void broadcast(const AbstractMessage &message);

    private:
        std::vector<std::string> subscriptions;

        void onReceive(const Packet &packet) override;
        void onReconnect() override;

        virtual void onReceive(const AbstractMessage &message) = 0;
};

#endif
