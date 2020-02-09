#include <bitcoinrpc/RPCConnection.h>
#include <stratum/StratumServer.h>
#include <mining/CoinbaseOutput.h>
#include <mining/hashplugin.h>

#include <util/CommandLineArguments.h>
#include <util/logger.h>

#include <iostream>


#define MINE_ADDRESS    "faf51e196d9190efb9892a55533c89e71c37b6c0"

class StratumInitializer : public StratumServer::NetworkStateInitializer
{
    public:
        inline StratumInitializer(RPCConnection &rpc) : rpc(rpc) {}

        NetworkStateRef getNetworkState() const override;
        void populateCoinbaseOutputs(CoinbaseOutputs outputs) const override;

    private:
        RPCConnection &rpc;
};

NetworkStateRef StratumInitializer::getNetworkState() const
{
    auto blockTemplate = this->rpc.getBlockTemplate();

    return std::make_shared<NetworkState>(
        blockTemplate.version,
        blockTemplate.miningTargetBits,
        blockTemplate.notBefore,
        blockTemplate.height,
        blockTemplate.previousBlockHash,
        blockTemplate.coinbaseCoins
    );
}

void StratumInitializer::populateCoinbaseOutputs(CoinbaseOutputs outputs) const
{
    outputs.push_back(std::make_shared<CoinbaseOutput>(CoinbaseOutput::p2pkh(ByteString::fromHex(MINE_ADDRESS), 1)));
}

int main(int argc, char *argv[])
{
    __loglevel = DEBUG;

    std::string rpcHostname = "127.0.0.1";
    std::string rpcUsername;
    std::string rpcPassword;
    int rpcPort = 42068;

    int stratumPort = 3032;
    std::string coinbaseSignature = "/FreshGRLC.net/NG/";

    std::string miningAlgorithm = "sha256d";

    CommandLineArguments(argc, argv, {
        { stratumPort,          "port",                 'p',        "Port to listen on for incoming stratum connections." },
        { rpcHostname,          "rpc-hostname",         'h',        "Hostname used to connect to the full node RPC interface." },
        { rpcPort,              "rpc-port",                         "Port used to connect to the full node RPC interface." },
        { rpcUsername,          "rpc-user",             'u', true,  "Username for full node RPC interface authentication." },
        { rpcPassword,          "rpc-password",         'P', true,  "Password for full node RPC interface authentication." },
        { coinbaseSignature,    "coinbase-signature",   'S',        "Signature to encode in coinbase transactions." },
        { miningAlgorithm,      "algorithm",            'a',        "Mining algorithm." }
    }).processOrPrintUsageAndExit();

    try
    {
        RPCConnection rpc(rpcUsername, rpcPassword, rpcHostname, rpcPort);
        StratumServer server(Listener(stratumPort), StratumInitializer(rpc), get_hashplugin(miningAlgorithm), coinbaseSignature);

        return server.listen();
    }
    catch (const std::runtime_error &e)
    {
        mlog(CRITICAL, "%s", e.what());

        std::cerr << std::endl;
        std::cerr << "Fatal error or unhandled exception: " << e.what() << std::endl;
        return 1;
    }
}
