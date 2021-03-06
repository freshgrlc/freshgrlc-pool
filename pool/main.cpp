#include "DaemonConnector.h"
#include "StratumConfiguration.h"

#include <iostream>
#include <unistd.h>

#include <mining/CoinbaseOutput.h>
#include <mining/hashplugin.h>
#include <stratum/StratumServer.h>

#include <util/CommandLineArguments.h>
#include <util/logger.h>


#define MINE_ADDRESS    "faf51e196d9190efb9892a55533c89e71c37b6c0"


static void enableDebugLogging()
{
    extern enum LogLevel __loglevel_mining;
    extern enum LogLevel __loglevel_bitcoinrpc;
    extern enum LogLevel __loglevel_stratum;

    __loglevel = DEBUG;

    __loglevel_mining = DEBUG;
    __loglevel_bitcoinrpc = DEBUG;
    __loglevel_stratum = DEBUG;
}

int main(int argc, char *argv[])
{
    std::string rpcHostname = "127.0.0.1";
    std::string rpcUsername;
    std::string rpcPassword;
    int rpcPort = 42068;

    int stratumPort = 3032;
    std::string coinbaseSignature = "/FreshGRLC.net/NG/";

    std::string miningAlgorithm = "sha256d";

    bool debug = false;

    CommandLineArguments(argc, argv, {
        { stratumPort,          "port",                 'p',        "Port to listen on for incoming stratum connections." },
        { rpcHostname,          "rpc-hostname",         'h',        "Hostname used to connect to the full node RPC interface." },
        { rpcPort,              "rpc-port",                         "Port used to connect to the full node RPC interface." },
        { rpcUsername,          "rpc-user",             'u', true,  "Username for full node RPC interface authentication." },
        { rpcPassword,          "rpc-password",         'P', true,  "Password for full node RPC interface authentication." },
        { coinbaseSignature,    "coinbase-signature",   'S',        "Signature to encode in coinbase transactions." },
        { miningAlgorithm,      "algorithm",            'a',        "Mining algorithm." },
        { debug,                "debug",                'D',        "Enable debug logging." }
    }).processOrPrintUsageAndExit();

    if (debug)
        enableDebugLogging();

    try
    {
        StratumConfiguration configuration(stratumPort, miningAlgorithm, coinbaseSignature, 10.0);
        DaemonConnector daemon(rpcUsername, rpcPassword, rpcHostname, rpcPort);
        StratumServer server(Listener(configuration.port), daemon.stratumInitializer(), daemon.blockSubmitter(), get_hashplugin(configuration.hashingAlgorithm), configuration.coinbaseSignature);

        daemon.registerUpdateNotificationsFor(server);

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
