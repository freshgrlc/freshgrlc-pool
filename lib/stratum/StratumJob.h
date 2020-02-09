#ifndef STRATUM_STRATUMJOB_H
#define STRATUM_STRATUMJOB_H

#include <string>
#include <stdint.h>

#include <mining/CoinbaseTransaction.h>
#include <mining/MerkleBranch.h>

#include "NetworkState.h"


#ifndef INCLUDE_NLOHMANN_JSON_HPP_
namespace nlohmann {
    class json;
};
#endif

using json = nlohmann::json;


class StratumJob
{
    public:
        StratumJob(uint32_t id, double diff, const NetworkStateRef &networkState, const CoinbaseTransactionRef &coinbase, const MerkleBranchRef &merkleBranch);

        json toJson(bool force) const;

        inline uint32_t id(void) const              { return _id; }
        inline double diff(void) const              { return _diff; }
        inline uint32_t creationTime(void) const    { return _creationTime; }

    private:
        uint32_t _id;
        double _diff;
        uint32_t _creationTime;

        NetworkStateRef networkState;
        CoinbaseTransactionRef coinbase;
        MerkleBranchRef merkleBranch;
};

#endif
