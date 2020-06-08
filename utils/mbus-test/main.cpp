#include "TestMessage.h"
#include "DummyClient.h"

#include <util/logger.h>

#include <unistd.h>

int main()
{
    extern LogLevel __loglevel_messagebus;
    extern LogLevel __loglevel_network;

    __loglevel_messagebus = DEBUG;
    __loglevel_network = DEBUG;

    DummyClient client("127.0.0.1");

    TestMessage message("Hello from " + std::to_string(getpid()));

    client.broadcast(message);

    sleep(1);
    client.subscribe(TestMessage::CLASS);

    for (int i = 0; i < 10; i++)
    {
        sleep(1);
        client.broadcast(message);
    }

    sleep(1);
    return 0;
}




