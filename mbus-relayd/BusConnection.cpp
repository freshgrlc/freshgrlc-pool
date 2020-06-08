#include "BusConnection.h"
#include "MessageBusServer.h"

#include <messagebus/OpaqueMessage.h>
#include <messagebus/MessageStreamParser.h>
#include <messagebus/SetSubscriptionsMessage.h>
#include <util/logger.h>


BusConnection::BusConnection(MessageBusServer &server, SocketBase &&socket) : IncomingConnection(server, std::move(socket)),
    parent(server),
    messageQueue(),
    worker(*this)
{
    mlog(INFO, "New client %p", this);

    this->setStreamParser(MessageStreamParser());
}

BusConnection::~BusConnection(void)
{
    mlog(INFO, "Client %p disconnected", this);
}

void BusConnection::queueMessageIfSubscribed(const OpaqueMessage &message)
{
    if (this->isSubscribedTo(message.messageClass()))
        this->messageQueue.queue(OpaqueMessage(message));
}

void BusConnection::onReceive(const Packet &packet)
{
    MessageRef message = AbstractMessage::decodeOpaque(packet);

    if (message->messageClass() == "__meta__")
    {
        try
        {
            MessageRef message = AbstractMessage::decode(packet);

            try
            {
                this->processProtocolMetaMessage(*message);
            }
            catch (std::exception &e)
            {
                mlog(ERROR, "Failed to handle message bus protocol meta message '%s' (%s), dropping", message->messageType().c_str(), e.what());
            }
        }
        catch (std::exception &e)
        {
            mlog(WARNING, "Received unsupported message bus protocol meta message (%s), dropping", e.what());
        }
    }
    else
    {
        OpaqueMessage &opaqueMessage = *((OpaqueMessage *) message.get());
        this->parent.broadcastToClients(opaqueMessage);
    }
}

void BusConnection::processProtocolMetaMessage(const AbstractMessage &message)
{
    if (message.is<SetSubscriptionsMessage>())
    {
        mlog(DEBUG, "Updating subscriptions for client %p", this);

        OBTAIN_LOCK(subscriptionsLock);

        this->subscriptions = message.as<SetSubscriptionsMessage>().subscriptions();
        return;
    }

    throw std::runtime_error("Unsupported protocol meta message '" + message.messageType() + "'");
}

bool BusConnection::isSubscribedTo(const std::string &messageClass)
{
    OBTAIN_LOCK(subscriptionsLock);

    for (auto &entry : this->subscriptions)
    {
        if (entry == messageClass)
            return true;
    }

    return false;
}

void BusConnection::Worker::processEvent(const OpaqueMessageRef &event)
{
    this->parent.send((Packet) event->encode());
}
