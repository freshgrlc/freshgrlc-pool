#include "NodeConnection.h"
#include "ConnectionManager.h"

#include <util/logger.h>


void NodeConnection::processMessage(const P2PMessageRef &message)
{
    this->P2PConnection::processMessage(message);

    switch (message->type())
    {
        case P2PMessage::VERACK:
            mlog(INFO, "Handshake with %s complete", this->host().c_str());
            break;

        case P2PMessage::ADDR:
            mlog(INFO, "Received address list from node %s:", this->host().c_str());

            for (int idx = 1; idx < message->payload().size(); idx += 30)
            {
                static const uint8_t ipv4_prefix[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff };
                const uint64_t *services = (uint64_t *) &message->payload()[idx+4];
                const uint8_t *ip = &message->payload()[idx+12];
                const uint16_t port = message->payload()[idx+28] * 0x100 | message->payload()[idx+29];

                std::string host;

                if (!memcmp(ip, ipv4_prefix, sizeof(ipv4_prefix)))
                    host =  std::to_string(ip[sizeof(ipv4_prefix)])   + "." +
                            std::to_string(ip[sizeof(ipv4_prefix)+1]) + "." +
                            std::to_string(ip[sizeof(ipv4_prefix)+2]) + "." +
                            std::to_string(ip[sizeof(ipv4_prefix)+3]);
                else
                    host = ConstByteStringRef(ip, 16).asHexPretty("[", "]", 2, ':');

                mlog(INFO, " [%2d] %s:%d (services: 0x%04lx)", idx / 30 + 1, host.c_str(), port, *services);

                this->parent.addNode(host, port);
            }

            int connectedNodes = this->parent.connectedNodes();
            int totalNodes = this->parent.totalNodes();
            int nodesWaiting = this->parent.nodesWaitingForConnection();

            mlog(INFO, "============================");
            mlog(INFO, "   Total nodes:      %4d", totalNodes);
            mlog(INFO, "   Connected nodes:  %4d", connectedNodes);
            mlog(INFO, "   Connection queue: %4d", nodesWaiting);
            mlog(INFO, "============================");
            break;
    }
}
