#include <nlohmann/json.hpp>

#include "StratumCall.h"

#include <logger.h>


#include "calls/MiningAuthorize.h"
#include "calls/MiningExtraNonceSubscribe.h"
#include "calls/MiningSubmit.h"
#include "calls/MiningSubscribe.h"


using namespace stratum::messages;


StratumCall::StratumCall(int messageId, const std::string &method) :
    messageId(messageId),
    method(method)
{
}

#define CHECK_AND_RETURN_IF_MATCHED(_class) \
    if (method == _class::METHOD) \
        return std::make_unique<_class>(msgId, params)

std::unique_ptr<StratumCall> StratumCall::parseMessage(const json &encoded)
{
    auto msgId = encoded["id"].get<int>();
    auto method = encoded["method"].get<std::string>();
    auto params = encoded["params"];

    if (!params.is_array())
        return nullptr;

    CHECK_AND_RETURN_IF_MATCHED(MiningAuthorize);
    CHECK_AND_RETURN_IF_MATCHED(MiningExtraNonceSubscribe);
    CHECK_AND_RETURN_IF_MATCHED(MiningSubmit);
    CHECK_AND_RETURN_IF_MATCHED(MiningSubscribe);

    return nullptr;
}
