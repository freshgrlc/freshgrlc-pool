#ifndef BITCOINRPC_RPCCONNECTION_H
#define BITCOINRPC_RPCCONNECTION_H

#include <string>
#include <memory>

#include <mining/BlockHeader.h>
#include <util/ByteString.h>

#include "RPCSocket.h"

#include "BlockChainInfo.h"
#include "BlockTemplate.h"


#ifndef INCLUDE_NLOHMANN_JSON_HPP_
namespace nlohmann {
    class json;
};
#define JSON_PARAMS_INITIALIZER
#else
#define JSON_PARAMS_INITIALIZER     = json::array()
#endif

using json = nlohmann::json;


class RPCConnection
{
    public:
        RPCConnection(const std::string &username, const std::string &password, const std::string &host, int port = 42068);

        BlockChainInfo getBlockChainInfo(void);
        BlockTemplate getBlockTemplate(void);

        ByteString getBlock(const ConstByteStringRef &blockhash);
        std::vector<ByteString> getTransactionsInBlock(const ConstByteStringRef &blockhash);

        void submitBlock(const BlockHeader &header, const ByteString &coinbaseTransaction, const RawTransactions &transactions);

    private:
        const std::string _host;
        const int _port;
        RPCSocketRef _socket;

        std::string _authkey;

        bool connectIfNotConnected(void);
        std::unique_ptr<json> sendAndReceive(const json &request);
        std::unique_ptr<json> sendAndReceivePayload(const std::string &method, const json &params JSON_PARAMS_INITIALIZER, std::string *error = 0);

        static std::string makeAuthKey(const std::string &username, const std::string &password);

        static void assertBlockhash(const ConstByteStringRef &blockhash);
};

#endif
