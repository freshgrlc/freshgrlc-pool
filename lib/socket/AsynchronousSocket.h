#ifndef SOCKET_ASYNCHRONOUSSOCKET_H
#define SOCKET_ASYNCHRONOUSSOCKET_H

#include "Socket.h"

#include <util/Thread.h>


class AsynchronousSocket : public Socket
{
    public:
        inline AsynchronousSocket(SocketBase &&socket, bool startReceiver = false) : Socket(std::move(socket)),
            receiver(this)
        {
            if (startReceiver)
                this->enableProcessing();
        }

        inline void enableProcessing(void)  { this->receiver.start(); }
        inline bool receiverRunning(void)   { return this->receiver.state() == ReceiverThread::Running; }

    protected:
        class ReceiverThread : public Thread
        {
            public:
                inline ReceiverThread(AsynchronousSocket *parent) : Thread(), parent(*parent) {}

            private:
                AsynchronousSocket &parent;

                void main(void) override;
                void initializationCallback(int error) override;
                void cleanupCallback(void) override;
        };

        ReceiverThread receiver;

        inline virtual void cleanup(void) {}
};

#endif
