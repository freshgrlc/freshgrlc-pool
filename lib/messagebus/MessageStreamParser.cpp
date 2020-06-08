#include "MessageStreamParser.h"
#include "AbstractMessage.h"


size_t MessageStreamParser::getReceivedMessageSize(const ConstByteStringRef &buffer)
{
    if (buffer.length < sizeof(uint32_t))
        return 0;

    auto reader = buffer.reader();

    uint32_t packetSize = reader.read<uint32_t>();

    if (packetSize > AbstractMessage::MAX_MESSAGE_SIZE)
        throw std::runtime_error("Received invalid message bus packet: message too long (" + std::to_string(packetSize) + ")");

    if (reader.remaining() < packetSize)
        return 0;

    return sizeof(uint32_t) + packetSize;
}
