#include <iostream>
#include <stdexcept>
#include <unistd.h>

#include <util/ByteString.h>
#include <util/CommandLineArguments.h>
#include <util/logger.h>

#include "ConnectionManager.h"

#define DEFAULT_NETWORK_MAGIC   "d2c6b6db"

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

        mlog(DEBUG, "Test");
        manager.addNode(host, port);

        for (;;)
        {
            sleep(60);

            auto nodeList = manager.getNodeList();
            int toTry = manager.nodesWaitingForConnection();
            int notAvailable = manager.totalNodes() - nodeList.size();

            mlog(INFO, "==============================================");
            mlog(INFO, "|                 NODE DUMP                  |");
            mlog(INFO, "==============================================");

            for (std::string &node : nodeList)
                mlog(INFO, "| %s", node.c_str());

            mlog(INFO, "|");
            mlog(INFO, "+ %d unavailable nodes (%d yet to try)", notAvailable, toTry);

            mlog(INFO, "==============================================");
        }
    }
    catch (const std::runtime_error &e)
    {
        mlog(CRITICAL, "%s", e.what());

        std::cerr << std::endl;
        std::cerr << "Fatal error or unhandled exception: " << e.what() << std::endl;
        return 1;
    }
}
