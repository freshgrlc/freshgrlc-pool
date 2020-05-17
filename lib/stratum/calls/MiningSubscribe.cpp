#include <nlohmann/json.hpp>

#include "MiningSubscribe.h"

#include <StratumClientConnection.h>
#include <StratumJob.h>
#include <StratumServer.h>


using namespace stratum::messages;

const std::string MiningSubscribe::METHOD = "mining.subscribe";

MiningSubscribe::MiningSubscribe(int messageId, const json &params) : StratumCall(messageId, METHOD),
    client(getParam<std::string>(params, 0, ""))
{
}

void MiningSubscribe::process(StratumClientConnection &connection)
{
    std::string connId = connection.connectionId();

    json reply = {
        json::array({
            {
                "mining.set_difficulty",
                connId
            },
            {
                "mining.notify",
                connId
            },
        }),
        ConstByteStringRef::raw(connection.extraNonce1()).asHex(),
        connection.jobManager().extraNonce2Size
    };

    connection.sendReply(*this, reply);
    connection.sendJob();
}
