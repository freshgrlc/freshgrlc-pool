#ifndef STRATUM_MININGSUBSCRIBE_H
#define STRATUM_MININGSUBSCRIBE_H

#include "../StratumCall.h"

namespace stratum { namespace messages
{

class MiningSubscribe : public StratumCall
{
    public:
        static const std::string METHOD;

        const std::string client;

        MiningSubscribe(int messageId, const json &params);

        virtual void process(StratumClientConnection &connection) override;
};

}}

#endif
