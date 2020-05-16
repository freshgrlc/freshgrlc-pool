#include "Version.h"

#include <cstring>
#include <ctime>


struct message
{
    int32_t     version;
    uint64_t    services;
    int64_t     timestamp;

    uint64_t    their_services;
    char        their_ip[16];
    uint16_t    their_port;

    uint64_t    my_services;
    char        my_ip[16];
    uint16_t    my_port;
    uint64_t    nonce;

    uint8_t     user_agent_size[1];
    //char      user_agent;
    int32_t     chain_height;
    int32_t     relaying_node;
};


using namespace bitcoinp2p::messages;


Version::Version(P2PMessage &&data) : P2PMessage(std::move(data))
{
}

Version::Version(uint32_t protocolVersion) : P2PMessage(Type)
{
    message msg;

    memset(&msg, 0, sizeof(msg));

    msg.relaying_node = 1;
    msg.timestamp = std::time(NULL);
    msg.version = protocolVersion;

    _payload << msg;
}

