#include "MessageBusServer.h"
#include "BusConnection.h"

#include <messagebus/OpaqueMessage.h>
#include <util/logger.h>


MessageBusServer::MessageBusServer(int port) : ListeningConnectionManager(Listener(port))
{
}

IncomingConnectionRef MessageBusServer::makeConnection(SocketBase &&socket)
{
    return std::make_unique<BusConnection>(*this, std::move(socket));
}

void MessageBusServer::broadcastToClients(const OpaqueMessage &message)
{
    auto &connections = this->connections().getAs<BusConnection>();

    mlog(DEBUG, "Broadcasting message to %2d/%2d nodes: [%s::%s] %s", this->calculateEffectiveBroadcastClients(message.messageClass()), connections.size(), message.messageClass().c_str(), message.messageType().c_str(), message.content.c_str());

    OBTAIN_FOREIGN_LOCK(this->connections().lock(), connections_lock);

    for (auto &connection : connections)
        connection->queueMessageIfSubscribed(message);
}

int MessageBusServer::calculateEffectiveBroadcastClients(const std::string &messageClass)
{
    OBTAIN_FOREIGN_LOCK(this->connections().lock(), connections_lock);

    int matching = 0;

    for (auto &connection : this->connections().getAs<BusConnection>())
    {
        if (connection->isSubscribedTo(messageClass))
            matching++;
    }

    return matching;
}
