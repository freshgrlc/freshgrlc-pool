#include <nlohmann/json.hpp>

#include "StratumJob.h"

#include <ctime>


template <typename int_t>
static std::string int2hex(int_t i)
{
    return ByteString((uint8_t *) &i, sizeof(i)).asHex();
}

StratumJob::StratumJob(uint32_t id, double diff, const NetworkStateRef &networkState, const CoinbaseTransactionRef &coinbase, const MerkleBranchRef &merkleBranch) :
    _id(id),
    _diff(diff),
    networkState(networkState),
    coinbase(coinbase),
    merkleBranch(merkleBranch)
{
    _creationTime = (uint32_t) std::time(NULL);

    if (_creationTime < networkState->miningStartTime)
        _creationTime = networkState->miningStartTime;
}

json StratumJob::toJson(bool force) const
{
    /*
     *  "params": [
     *      "8c6e",
     *      "4aab33fcf4579faee35bde813ad6f66f8e1c3fed97fb5763ac6de4d02b301277",
     *      "01000000010000000000000000000000000000000000000000000000000000000000000000ffffffff210472329000048b2c825d08",
     *      "0d2f6e6f64655374726174756d2f00000000020000000000000000266a24aa21a9ede2f61c3f71d1defd3fa999dfa36953755c690689799962b48bebd836974e8cf9cd1576270f0000001976a914f54c85705d38622a202c6f97caf7248cc4be001c88ac00000000",
     *      [],
     *      "20000202",
     *      "1908ef40",
     *      "5d822c8b",
     *      true
     *  ]
     */

    return json::array({
        int2hex(this->id()),
        this->networkState->previousBlock.bytes().asHex(),
        this->coinbase->part1().asHex(),
        this->coinbase->part2().asHex(),
        this->merkleBranch->asHexArray(),
        int2hex(this->networkState->version),
        int2hex(this->networkState->bits),
        int2hex(this->creationTime()),
        force
    });
}


