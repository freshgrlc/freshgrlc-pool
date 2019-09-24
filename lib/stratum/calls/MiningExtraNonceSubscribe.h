#ifndef STRATUM_MININGEXTRANONCESUBSCRIBE_H
#define STRATUM_MININGEXTRANONCESUBSCRIBE_H

#include "../StratumCall.h"

namespace stratum { namespace messages
{

class MiningExtraNonceSubscribe : public StratumCall
{
    public:
        static const std::string METHOD;

        MiningExtraNonceSubscribe(int messageId, const json &params);

        virtual void process(StratumClientConnection &connection) override;
};

}}

#endif
