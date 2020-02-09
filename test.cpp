#include <bitcoinrpc/RPCConnection.h>
#include <stratum/StratumServer.h>
#include <mining/CoinbaseOutput.h>

#include <util/logger.h>


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

    RPCConnection rpc(argv[1], argv[2], "127.0.0.1", 42088);
    StratumServer server(Listener(3032), StratumInitializer(rpc), "/FreshGRLC.net/NG/");

    return server.listen();
}
