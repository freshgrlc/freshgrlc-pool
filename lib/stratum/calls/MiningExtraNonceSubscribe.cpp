#include <nlohmann/json.hpp>

#include "MiningExtraNonceSubscribe.h"

#include <StratumClientConnection.h>
#include <StratumJob.h>
#include <StratumServer.h>


using namespace stratum::messages;

const std::string MiningExtraNonceSubscribe::METHOD = "mining.extranonce.subscribe";

MiningExtraNonceSubscribe::MiningExtraNonceSubscribe(int messageId, const json &params) : StratumCall(messageId, METHOD)
{
}

void MiningExtraNonceSubscribe::process(StratumClientConnection &connection)
{
    connection.sendReply(*this, (json) false, (json) "Not supported");
    connection.sendJob();
}
