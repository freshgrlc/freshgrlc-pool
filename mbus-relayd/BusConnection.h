#ifndef BUSCONNECTION_H
#define BUSCONNECTION_H

#include <socket/IncomingConnection.h>
#include <util/Queue.h>
#include <util/WorkerThread.h>


class AbstractMessage;
class OpaqueMessage;
class MessageBusServer;

class BusConnection : public IncomingConnection
{
    public:
        BusConnection(MessageBusServer &server, SocketBase &&socket);
        virtual ~BusConnection(void);

        void queueMessageIfSubscribed(const OpaqueMessage &message);
        bool isSubscribedTo(const std::string &messageClass);

    private:
        class Worker : public WorkerThread<OpaqueMessage>
        {
            public:
                inline Worker(BusConnection &parent) : WorkerThread(parent.messageQueue), parent(parent) {}

            private:
                BusConnection &parent;

                void processEvent(const WorkerThread::EventRef &event) override;
        };

        MessageBusServer &parent;
        Queue<OpaqueMessage> messageQueue;
        Worker worker;
        Lock subscriptionsLock;
        std::vector<std::string> subscriptions;

        void onReceive(const Packet &packet) override;

        void processProtocolMetaMessage(const AbstractMessage &message);
};

#endif
