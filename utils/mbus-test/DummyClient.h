#ifndef DUMMYCLIENT_H
#define DUMMYCLIENT_H

#include <messagebus/MessageBusClient.h>


class DummyClient : public MessageBusClient
{
    public:
        using MessageBusClient::MessageBusClient;

    private:
        void onReceive(const AbstractMessage &message) override;
};

#endif
