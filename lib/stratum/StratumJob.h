#ifndef STRATUM_STRATUMJOB_H
#define STRATUM_STRATUMJOB_H

#include <string>
#include <stdint.h>

#include <mining/CoinbaseTransaction.h>
#include <mining/MerkleBranch.h>
#include <mining/RawTransactions.h>
#include <interfaces/plugins/HashPlugin.h>

#include "BlockGenerationInformation.h"


#ifndef INCLUDE_NLOHMANN_JSON_HPP_
namespace nlohmann {
    class json;
};
#endif

using json = nlohmann::json;


class BlockSubmitter;

class StratumJob
{
    public:
        class validation_error : public std::runtime_error
        {
            public:
                using runtime_error::runtime_error;
        };

        StratumJob(uint32_t id, double diff, const BlockGenerationInformationRef &generationInformation, const CoinbaseTransactionRef &coinbase, HashPluginRef hasher);
        StratumJob(const StratumJob &them);

        json toJson(bool force) const;

        bool checkSolution(uint32_t time, uint32_t nonce, CoinbaseTransaction::nonce1_t extraNonce1, CoinbaseTransaction::nonce2_t extraNonce2, BlockSubmitter &blockSubmitter, const char *logId);

        inline uint32_t id(void) const              { return _id; }
        inline double diff(void) const              { return _diff; }
        inline uint32_t creationTime(void) const    { return _creationTime; }

    private:
        uint32_t _id;
        double _diff;
        uint32_t _creationTime;

        BlockGenerationInformationRef generationInformation;
        CoinbaseTransactionRef coinbase;

        HashPluginRef hasher;
};

#endif
