#ifndef MESSAGEBUS_MESSAGESTREAMPARSER_H
#define MESSAGEBUS_MESSAGESTREAMPARSER_H

#include <socket/Socket.h>

class MessageStreamParser : public Socket::StreamParser
{
    private:
        size_t getReceivedMessageSize(const ConstByteStringRef &buffer) override;
};

#endif
