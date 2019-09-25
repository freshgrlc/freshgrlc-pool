#include <nlohmann/json.hpp>

#include "RPCConnection.h"

#include <socket/ErrnoError.h>

#include <openssl/evp.h>
#include <cstring>

RPCConnection::RPCConnection(const std::string &username, const std::string &password, const std::string &host, int port) : Socket(openConnectionTo(host, port)),
    _authkey(makeAuthKey(username, password))
{
}

RPCConnection::BlockChainInfo RPCConnection::getBlockChainInfo()
{
    auto _response = this->sendAndReceivePayload("getblockchaininfo");

    if (!_response)
        throw std::runtime_error("getblockchaininfo RPC failure");

    auto &response = *_response.get();

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

std::unique_ptr<json> RPCConnection::sendAndReceive(const json &request)
{
    if (!this->connected())
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

    this->send(Packet(data));

    this->response = "";

    while (this->connected())
        this->receive();

    const char *dataStart = strstr(this->response.c_str(), "\r\n\r\n");

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

    std::unique_ptr<json> response = this->sendAndReceive(envelope);

    if (!response)
        return nullptr;

    if (response->contains("error") && response->operator[]("error").is_string())
    {
        const json &jsonError = response->operator[]("error");

        if (error)
            *error = jsonError.get<std::string>();
        else
            mlog(ERROR, "RPC returned error for '%s' call: %s", method.c_str(), jsonError.get<std::string>().c_str());
    }

    if (!response->contains("result"))
        return nullptr;

    return std::make_unique<json>(response->operator[]("result"));
}


void RPCConnection::onReceive(const Packet &packet)
{
    this->response += packet.asString();
}

std::string RPCConnection::makeAuthKey(const std::string &username, const std::string &password)
{
    std::string raw = username + ":" + password;
    uint8_t encoded[(raw.length() + 2) / 3 * 4 + 1];

    EVP_EncodeBlock(encoded, (const uint8_t *) raw.c_str(), raw.length());

    return (const char *) encoded;
}
