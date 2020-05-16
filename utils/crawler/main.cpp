#include <iostream>
#include <unistd.h>

#include <bitcoinp2p/P2PConnection.h>

#include <util/ByteString.h>
#include <util/CommandLineArguments.h>
#include <util/logger.h>

#include <bitcoinp2p/messages/GetAddr.h>


#define DEFAULT_NETWORK_MAGIC   "d2c6b6db"


using namespace bitcoinp2p;


class ConnectionManager;

class NodeConnection : public P2PConnection
{
    public:
        inline NodeConnection(const std::string &host, int port, const ConstByteStringRef &magic, ConnectionManager *parent) : P2PConnection(host, port, magic), parent(parent), _connected(false) {}

        inline bool connected(void) { return _connected; }

    private:
        ConnectionManager * const parent;
        bool _connected;

        inline void handshakeComplete() override
        {
            _connected = true;
            this->sendMessage(messages::GetAddr());
        }

        void processMessage(const P2PMessageRef &message) override;
};

class ConnectionManager : public std::vector<std::unique_ptr<NodeConnection>>
{
    public:
        inline ConnectionManager(ByteString &&magic) : magic(std::move(magic)) {}

        inline void addConnection(const std::string &host, uint16_t port)
        {
            mlog(INFO, "Connecting to %s:%d...", host.c_str(), port);
            this->push_back(std::make_unique<NodeConnection>(host, port, this->magic, this));
        }

        inline void addConnectionIfNotExists(const std::string &host, uint16_t port)
        {
            OBTAIN_LOCK(lock);

            for (auto &conn : *this)
            {
                if (conn->host() == host && conn->port() == port)
                    return;
            }

            this->addConnection(host, port);
        }

        inline int totalNodes(void)
        {
            return this->size();
        }

        inline int connectedNodes(void)
        {
            int connectedNodes = 0;

            OBTAIN_LOCK(lock);

            for (auto &conn : *this)
            {
                if (conn->connected())
                    connectedNodes++;
            }

            return connectedNodes;
        }

    private:
        Lock lock;
        const ByteString magic;
};

void NodeConnection::processMessage(const P2PMessageRef &message)
{
    this->P2PConnection::processMessage(message);

    if (message->type() == P2PMessage::ADDR)
    {
        mlog(INFO, "Received address list from node:");

        for (int idx = 1; idx < message->payload().size(); idx += 30)
        {
            static const uint8_t ipv4_prefix[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff };
            const uint64_t *services = (uint64_t *) &message->payload()[idx+4];
            const uint8_t *ip = &message->payload()[idx+12];
            const uint16_t port = message->payload()[idx+28] * 0x100 | message->payload()[idx+29];

            if (!memcmp(ip, ipv4_prefix, sizeof(ipv4_prefix)))
            {
                mlog(INFO, " [%2d] %d.%d.%d.%d:%d (services: 0x%014lx)", idx / 30 + 1, ip[12], ip[13], ip[14], ip[15], port, *services);

                std::string host = std::to_string(ip[12]) + "." + std::to_string(ip[13]) + "." + std::to_string(ip[14]) + "." + std::to_string(ip[15]);
                parent->addConnectionIfNotExists(host, port);
            }
            else
                mlog(INFO, " [%2d] %s:%d (services: 0x%04lx)", idx / 30 + 1, ConstByteStringRef(ip, 16).asHexPretty("[", "]", 2).c_str(), port, *services);
        }

        mlog(INFO, "===========================");
        mlog(INFO, "   Total nodes:     %4d", this->parent->totalNodes());
        mlog(INFO, "   Connected nodes: %4d", this->parent->connectedNodes());
        mlog(INFO, "===========================");
    }
}

static void enableDebugLogging()
{
    extern enum LogLevel __loglevel_network;
    extern enum LogLevel __loglevel_bitcoinp2p;

    __loglevel = DEBUG;
    __loglevel_network = DEBUG;
    __loglevel_bitcoinp2p = DEBUG;
}

int main(int argc, char *argv[])
{
    std::string host;
    std::string magicHex = DEFAULT_NETWORK_MAGIC;
    int port = 42069;

    bool debug = false;

    CommandLineArguments(argc, argv, {
        { host,                 "hostname",             'h', true,  "Hostname used for initial connection." },
        { port,                 "port",                 'P',        "Port used for initial connection." },
        { magicHex,             "magic",                'M',        "4-byte network magic (in hex, without prefix)." },
        { debug,                "debug",                'D',        "Enable debug logging." }
    }).processOrPrintUsageAndExit();

    if (debug)
        enableDebugLogging();

    ByteString magic = ByteString::fromHex(magicHex);

    if (magic.size() != 4)
    {
        std::cerr << "Network magic needs to be 4 bytes, e.g. " DEFAULT_NETWORK_MAGIC << std::endl;
        return 1;
    }

    try
    {
        ConnectionManager manager(std::move(magic));

        manager.addConnection(host, port);

        for (;;) pause();
    }
    catch (const std::runtime_error &e)
    {
        mlog(CRITICAL, "%s", e.what());

        std::cerr << std::endl;
        std::cerr << "Fatal error or unhandled exception: " << e.what() << std::endl;
        return 1;
    }
}

