#ifndef STRATUM_MININGSUBMIT_H
#define STRATUM_MININGSUBMIT_H

#include "../StratumCall.h"

#include <mining/CoinbaseTransaction.h>

namespace stratum { namespace messages
{

class MiningSubmit : public StratumCall
{
    public:
        typedef CoinbaseTransaction::nonce2_t nonce2_t;

        static const std::string METHOD;

        const std::string username;
        const uint32_t jobId;
        const nonce2_t extraNonce2;
        const uint32_t blockHeaderTime;
        const uint32_t blockHeaderNonce;

        MiningSubmit(int messageId, const json &params);

        virtual void process(StratumClientConnection &connection) override;
};

}}

#endif
