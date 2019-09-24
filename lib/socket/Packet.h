#ifndef SOCKET_PACKET_H
#define SOCKET_PACKET_H

#include <util/ConstByteStringRef.h>


class Packet : public ConstByteStringRef
{
    public:
        using ConstByteStringRef::ConstByteStringRef;
};

#endif
