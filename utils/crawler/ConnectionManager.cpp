#include "ConnectionManager.h"
#include "NodeConnection.h"

#include <util/logger.h>


ConnectionManager::ConnectionManager(ByteString &&magic) :
    magic(std::move(magic)),
    newNodeEventProcessors(*this)
{
}

void ConnectionManager::addNode(const std::string &host, uint16_t port)
{
    this->newNodeEventQueue.queue({ host, port });
}

bool ConnectionManager::containsNode(const std::string &host, uint16_t port)
{
    OBTAIN_LOCK(lock);
    return this->containsNodeUnSafe(host, port);
}

bool ConnectionManager::containsNodeUnSafe(const std::string &host, uint16_t port)
{
    for (auto &conn : *this)
    {
        if (conn.host() == host && conn.port() == port)
            return true;
    }

    return false;
}

ConnectionHolder &ConnectionManager::addConnection(const std::string &host, uint16_t port)
{
    OBTAIN_LOCK(lock);
    return this->addConnectionUnSafe(host, port);
}

ConnectionHolder &ConnectionManager::addConnectionUnSafe(const std::string &host, uint16_t port)
{
    mlog(INFO, "Connecting to %s:%d...", host.c_str(), port);

    this->emplace_back(*this, host, port);
    return (*this)[this->size()-1];
}

void ConnectionManager::processAddNodeEvent(const NewNodeEventProcessor::EventRef &event)
{
    ConnectionHolder *holder = NULL;

    {
        OBTAIN_LOCK(lock);

        if (this->containsNodeUnSafe(event->host, event->port))
        {
            mlog(DEBUG, "Ignoring duplicate node %s:%d", event->host.c_str(), event->port);
            return;
        }

        holder = &this->addConnectionUnSafe(event->host, event->port);
    }

    holder->connect();
    mlog(INFO, "Connected to %s:%d", event->host.c_str(), event->port);
}

int ConnectionManager::connectedNodes(void)
{
    int connectedNodes = 0;

    OBTAIN_LOCK(lock);

    for (auto &conn : *this)
    {
        if (conn.connected())
            connectedNodes++;
    }

    return connectedNodes;
}

std::vector<std::string> ConnectionManager::getNodeList(bool includeUnavailable)
{
    std::vector<std::string> list;

    OBTAIN_LOCK(lock);

    for (auto &conn : *this)
    {
        if (includeUnavailable || conn.connected())
            list.emplace_back(conn.host() + ":" + std::to_string(conn.port()));
    }

    return list;
}
