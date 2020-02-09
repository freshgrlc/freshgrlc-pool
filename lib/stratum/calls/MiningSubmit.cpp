#include <nlohmann/json.hpp>

#include "MiningSubmit.h"

#include <stratum/StratumClientConnection.h>
#include <stratum/StratumJob.h>

using namespace stratum::messages;

const std::string MiningSubmit::METHOD = "mining.submit";

MiningSubmit::MiningSubmit(int messageId, const json &params) : StratumCall(messageId, METHOD),
    username(getParam<std::string>(params, 0, "")),
    jobId(getParam<ByteString>(params, 1, ByteString()).parseUIntBE<uint32_t>()),
    extraNonce2(getParam<ByteString>(params, 2, ByteString()).parseUIntLE<nonce2_t>()),
    blockHeaderTime(getParam<ByteString>(params, 3, ByteString()).parseUIntBE<uint32_t>()),
    blockHeaderNonce(getParam<ByteString>(params, 4, ByteString()).parseUIntBE<uint32_t>())
{
}

void MiningSubmit::process(StratumClientConnection &connection)
{
    auto job = connection.getActiveJob(this->jobId);

    if (!job)
    {
        connection.sendReply(*this, false, std::string("Job ") + std::to_string(this->jobId) + " not or no longer valid");
        return;
    }

    try
    {
        job->checkSolution(this->blockHeaderTime, this->blockHeaderNonce, connection.extraNonce1(), this->extraNonce2);
        connection.sendReply(*this, true);
    }
    catch (const StratumJob::validation_error &e)
    {
        connection.sendReply(*this, false, e.what());
    }
}
