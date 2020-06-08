#include "MessageBusClient.h"
#include "MessageStreamParser.h"
#include "AbstractMessage.h"
#include "SetSubscriptionsMessage.h"

#include <util/logger.h>


const int MessageBusClient::DEFAULT_PORT = 9094;


MessageBusClient::MessageBusClient(const std::string &host, int port) : ReconnectingClient(host, port)
{
    this->setStreamParser(MessageStreamParser());
    this->enableProcessing();
}

void MessageBusClient::setSubscriptions(std::vector<std::string> &&messageClasses)
{
    /* NOTE: This isn't thread safe */

    if (messageClasses != this->subscriptions)
    {
        this->broadcast(SetSubscriptionsMessage(messageClasses));
        this->subscriptions = std::move(messageClasses);
    }
}

void MessageBusClient::subscribe(const std::string &messageClass)
{
    /* NOTE: This isn't thread safe */

    for (const std::string &existingClass : this->subscriptions)
    {
        if (existingClass == messageClass)
            return;
    }

    this->subscriptions.emplace_back(messageClass);
    this->broadcast(SetSubscriptionsMessage(this->subscriptions));
}

void MessageBusClient::unsubscribe(const std::string &messageClass)
{
    /* NOTE: This isn't thread safe */

    for (auto it = this->subscriptions.begin(); it != this->subscriptions.end(); it++)
    {
        if (*it == messageClass)
        {
            this->subscriptions.erase(it);
            this->broadcast(SetSubscriptionsMessage(this->subscriptions));
            return;
        }
    }
}

void MessageBusClient::broadcast(const AbstractMessage &message)
{
    this->send((Packet) message.encode());

    mlog(DEBUG, "<<< [%s::%s] %s", message.messageClass().c_str(), message.messageType().c_str(), message.raw().c_str());
}

void MessageBusClient::onReceive(const Packet &packet)
{
    try
    {
        MessageRef message = AbstractMessage::decode(packet);

        mlog(DEBUG, ">>> [%s::%s] %s", message->messageClass().c_str(), message->messageType().c_str(), message->raw().c_str());

        try
        {
            this->onReceive(*message);
        }
        catch (std::exception &e)
        {
            mlog(ERROR, "Failed to handle message bus message '%s::%s' (%s), dropping", message->messageClass().c_str(), message->messageType().c_str(), e.what());
        }
    }
    catch (std::exception &e)
    {
        mlog(WARNING, "Received unsupported message bus message (%s), dropping", e.what());
    }
}

void MessageBusClient::onReconnect()
{
    mlog(DEBUG, "Reconnected to bus, re-sending subscription info...");
    this->broadcast(SetSubscriptionsMessage(this->subscriptions));
}
