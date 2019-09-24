#ifndef STRATUM_MININGAUTHORIZE_H
#define STRATUM_MININGAUTHORIZE_H

#include "../StratumCall.h"

namespace stratum { namespace messages
{

class MiningAuthorize : public StratumCall
{
    public:
        static const std::string METHOD;

        const std::string username;
        const std::string password;

        MiningAuthorize(int messageId, const json &params);

        virtual void process(StratumClientConnection &connection) override;
};

}}

#endif
