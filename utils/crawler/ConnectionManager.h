#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <vector>
#include <memory>

#include <util/ByteString.h>
#include <util/Lock.h>
#include <util/logger.h>
#include <util/WorkerThread.h>

#include "ConnectionHolder.h"


class ConnectionManager : private std::vector<ConnectionHolder>
{
    public:
        ConnectionManager(ByteString &&magic);

        void addNode(const std::string &host, uint16_t port);
        bool containsNode(const std::string &host, uint16_t port);

        inline int totalNodes(void)     { return this->size(); }
        int connectedNodes(void);

        std::vector<std::string> getNodeList(bool includeUnavailable = false);

        inline int nodesWaitingForConnection(void)
                                        { return this->newNodeEventQueue.messagesWaiting(); }

        inline const ByteString &networkMagic(void)
                                        { return this->magic; }

    private:
        class NewNodeEvent
        {
            public:
                inline NewNodeEvent(const std::string &host, uint16_t port) : host(host), port(port) {}
                inline NewNodeEvent(std::string &&host, uint16_t port) : host(std::move(host)), port(port) {}
                inline NewNodeEvent(const NewNodeEvent &src) : host(src.host), port(src.port) {}
                inline NewNodeEvent(NewNodeEvent &&src) : host(std::move(src.host)), port(src.port) {}

                const std::string host;
                const uint16_t port;
        };

        class NewNodeEventProcessor : public WorkerThread<NewNodeEvent>
        {
            public:
                inline NewNodeEventProcessor(ConnectionManager &parent) : WorkerThread(parent.newNodeEventQueue), _parent(parent) {}

            private:
                ConnectionManager &_parent;

                inline void processEvent(const WorkerThread::EventRef &event) override
                {
                    _parent.processAddNodeEvent(event);
                }
        };

        class NewNodeEventProcessors
        {
            public:
                inline NewNodeEventProcessors(ConnectionManager &parent) :
                    processor1(parent),
                    processor2(parent),
                    processor3(parent),
                    processor4(parent)
                {}

                const NewNodeEventProcessor processor1;
                const NewNodeEventProcessor processor2;
                const NewNodeEventProcessor processor3;
                const NewNodeEventProcessor processor4;
        };

        Lock lock;
        const ByteString magic;
        NewNodeEventProcessor::EventQueue newNodeEventQueue;
        NewNodeEventProcessors newNodeEventProcessors;

        bool containsNodeUnSafe(const std::string &host, uint16_t port);

        ConnectionHolder &addConnection(const std::string &host, uint16_t port);
        ConnectionHolder &addConnectionUnSafe(const std::string &host, uint16_t port);
        void processAddNodeEvent(const NewNodeEventProcessor::EventRef &event);
};

#endif
