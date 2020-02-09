#include <nlohmann/json.hpp>

#include "RPCConnection.h"
#include "RPCSocket.h"

#include <socket/ErrnoError.h>

#include <openssl/evp.h>
#include <cstring>




RPCConnection::RPCConnection(const std::string &username, const std::string &password, const std::string &host, int port) :
    _host(host),
    _port(port),
    _socket(RPCSocket::connectToFor(host, port, *this)),
    _authkey(makeAuthKey(username, password))
{
}

BlockChainInfo RPCConnection::getBlockChainInfo()
{
    auto _response = this->sendAndReceivePayload("getblockchaininfo");

    if (!_response)
        throw std::runtime_error("getblockchaininfo RPC failure");

    auto &response = *_response;

    BlockChainInfo::ChainType chain;
    std::string chainName = response["chain"].get<std::string>();

    if (chainName == "main")
        chain = BlockChainInfo::Main;
    else if (chainName == "test")
        chain = BlockChainInfo::Testnet;
    else if (chainName == "regtest")
        chain = BlockChainInfo::RegTest;
    else
    {
        mlog(WARNING, "Unknown chain type '%s'", chainName.c_str());
        chain = BlockChainInfo::Unknown;
    }

    return BlockChainInfo(chain,
                          response["blocks"].get<int>(),
                          response["blocks"].get<int>() == response["headers"].get<int>(),
                          ByteString::fromHex(response["bestblockhash"].get<std::string>()));
}

BlockTemplate RPCConnection::getBlockTemplate()
{
    auto _response = this->sendAndReceivePayload("getblocktemplate", (json) { (json) {{ "rules", (json) { "segwit" }}}});

    if (!_response)
        throw std::runtime_error("getblocktemplate RPC failure");

    auto &response = *_response;
    BlockTemplate::Transactions transactions;

    for (auto &tx : response["transactions"])
    {
        BlockTemplate::TransactionInfo txInfo(
            ByteString::fromHex(tx["txid"].get<std::string>()),
            ByteString::fromHex(tx["data"].get<std::string>())
        );

        transactions.push_back(std::move(txInfo));
    }

    return BlockTemplate(
        response["version"].get<uint32_t>(),
        response["height"].get<uint32_t>(),
        ByteString::fromHex(response["previousblockhash"].get<std::string>()),
        std::move(transactions),
        ByteString::fromHex(response["default_witness_commitment"].get<std::string>()),
        response["coinbasevalue"].get<uint64_t>(),
        ByteString::fromHex(response["target"].get<std::string>()),
        *((uint32_t *) ByteString::fromHex(response["bits"].get<std::string>()).data()),
        response["mintime"].get<uint32_t>()
    );
}

ByteString RPCConnection::getBlock(const ConstByteStringRef &blockhash)
{
    assertBlockhash(blockhash);

    json params = {
        blockhash.asHex(),
        0
    };

    auto _response = this->sendAndReceivePayload("getblock", params);

    if (!_response || !_response->is_string())
        throw std::runtime_error("getblock RPC failure");

    return ByteString::fromHex(_response->get<std::string>());
}

std::vector<ByteString> RPCConnection::getTransactionsInBlock(const ConstByteStringRef &blockhash)
{
    assertBlockhash(blockhash);

    json params = {
        blockhash.asHex()
    };

    auto _response = this->sendAndReceivePayload("getblock", params);

    if (!_response || !_response->is_object() || !_response->contains("tx") || !(*_response)["tx"].is_array())
        throw std::runtime_error("getblock RPC failure");

    std::vector<ByteString> retval;

    for (auto &tx : (*_response)["tx"])
        retval.push_back(ByteString::fromHex(tx.get<std::string>()).reverse());

    return retval;
}

bool RPCConnection::connectIfNotConnected()
{
    if (!_socket.get() || !_socket->connected())
        _socket = RPCSocket::connectToFor(_host, _port, *this);

    return _socket.get() && _socket->connected();
}

std::unique_ptr<json> RPCConnection::sendAndReceive(const json &request)
{
    if (!this->connectIfNotConnected())
    {
        mlog(ERROR, "Cannot send request via Bitcoin RPC: Not connected");
        return nullptr;
    }

    std::string data =
        "POST / HTTP/1.0\n"
        "Content-Type: text/plain;\n";

    data += "Authorization: Basic " + _authkey + "\n";

    std::string payload = request.dump() + "\n";

    data += "Content-Length: " + std::to_string(payload.length()) + "\n\n";
    data += payload;

    std::string response = _socket->sendAndReceive(data);
    const char *dataStart = strstr(response.c_str(), "\r\n\r\n");

    if (!dataStart)
    {
        mlog(ERROR, "Received empty or invalid RPC response");
        return nullptr;
    }

    return std::make_unique<json>(json::parse(dataStart + 4));
}

std::unique_ptr<json> RPCConnection::sendAndReceivePayload(const std::string &method, const json &params, std::string *error)
{
    static int __counter = 0;
    json envelope = json::object();

    envelope["jsonrpc"] = "1.0";
    envelope["id"] = std::to_string(__counter++);
    envelope["method"] = method;
    envelope["params"] = params;

    std::unique_ptr<json> response_ptr = this->sendAndReceive(envelope);

    if (!response_ptr)
        return nullptr;

    json &response = *response_ptr;

    if (response.contains("error") && response["error"].is_object() &&
        response["error"].contains("message") && response["error"]["message"].is_string())
    {
        const json &jsonError = response["error"]["message"];

        if (error)
            *error = jsonError.get<std::string>();
        else
            mlog(ERROR, "RPC returned error for '%s' call: %s", method.c_str(), jsonError.get<std::string>().c_str());
    }

    if (!response.contains("result") || response["result"].is_null())
        return nullptr;

    return std::make_unique<json>(response["result"]);
}

std::string RPCConnection::makeAuthKey(const std::string &username, const std::string &password)
{
    std::string raw = username + ":" + password;
    uint8_t encoded[(raw.length() + 2) / 3 * 4 + 1];

    EVP_EncodeBlock(encoded, (const uint8_t *) raw.c_str(), raw.length());

    return (const char *) encoded;
}

void RPCConnection::assertBlockhash(const ConstByteStringRef &blockhash)
{
    if (blockhash.length != 32)
        throw std::runtime_error("Invalid blockhash: " + blockhash.asHex());
}

