#include "DummyClient.h"
#include "TestMessage.h"

#include <messagebus/AbstractMessage.h>
#include <util/logger.h>


void DummyClient::onReceive(const AbstractMessage &message)
{
    mlog(INFO, "Received message: %s::%s", message.messageClass().c_str(), message.messageType().c_str());

    if (message.is<TestMessage>())
        mlog(INFO, "Message text: %s", message.as<TestMessage>().message().c_str());
}

