#include <nlohmann/json.hpp>

#include "MiningAuthorize.h"

#include <StratumClientConnection.h>
#include <StratumJob.h>
#include <StratumServer.h>


using namespace stratum::messages;

const std::string MiningAuthorize::METHOD = "mining.authorize";

MiningAuthorize::MiningAuthorize(int messageId, const json &params) : StratumCall(messageId, METHOD),
    username(getParam<std::string>(params, 0, "")),
    password(getParam<std::string>(params, 1, ""))
{
}

void MiningAuthorize::process(StratumClientConnection &connection)
{
    connection.sendReply(*this, (json) true);
}
