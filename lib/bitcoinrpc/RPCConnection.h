#ifndef BITCOINRPC_RPCCONNECTION_H
#define BITCOINRPC_RPCCONNECTION_H

#include <string>
#include <memory>

#include <socket/Socket.h>
#include <util/ByteString.h>


#ifndef INCLUDE_NLOHMANN_JSON_HPP_
namespace nlohmann {
    class json;
};
#define JSON_PARAMS_INITIALIZER
#else
#define JSON_PARAMS_INITIALIZER     = json::array()
#endif

using json = nlohmann::json;


class RPCConnection : public Socket
{
    public:
        class BlockChainInfo
        {
            public:
                enum ChainType
                {
                    Main,
                    Testnet,
                    RegTest,

                    Unknown
                };

                const ChainType chain;
                const int height;
                const bool synced;
                const ByteString chaintip;

                inline BlockChainInfo(ChainType chain, int height, bool synced, ByteString &&chaintip) :
                    chain(chain),
                    height(height),
                    synced(synced),
                    chaintip(chaintip)
                {}
        };

        RPCConnection(const std::string &username, const std::string &password, const std::string &host, int port = 42068);

        BlockChainInfo getBlockChainInfo(void);

    private:
        std::string _authkey;
        std::string response;

        std::unique_ptr<json> sendAndReceive(const json &request);
        std::unique_ptr<json> sendAndReceivePayload(const std::string &method, const json &params JSON_PARAMS_INITIALIZER, std::string *error = 0);

        virtual void onReceive(const Packet &packet) override;

        static std::string makeAuthKey(const std::string &username, const std::string &password);
};

#endif
