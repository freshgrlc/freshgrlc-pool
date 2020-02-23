#include <nlohmann/json.hpp>

#include <mining/hashplugin.h>
#include <mining/MerkleBranch.h>
#include <mining/BlockHeader.h>
#include <mining/swap32.h>

#include <util/logger.h>

#include "StratumJob.h"
#include "BlockSubmitter.h"

#include <ctime>


template <typename int_t>
static std::string int2hex(int_t i, bool littleEndian = false)
{
    ByteString encoded((uint8_t *) &i, sizeof(i));

    if (littleEndian)
        return encoded.asHex();

    return encoded.reverse().asHex();
}

StratumJob::StratumJob(uint32_t id, double diff, const NetworkStateRef &networkState, const CoinbaseTransactionRef &coinbase, const RawTransactionsRef &transactions, const MerkleBranchRef &merkleBranch, HashPluginRef hasher) :
    _id(id),
    _diff(diff),
    networkState(networkState),
    coinbase(coinbase),
    transactions(transactions),
    merkleBranch(merkleBranch),
    hasher(hasher)
{
    _creationTime = (uint32_t) std::time(NULL);

    if (_creationTime < networkState->miningStartTime)
        _creationTime = networkState->miningStartTime;
}

StratumJob::StratumJob(const StratumJob &them) :
    _id(them._id),
    _diff(them._diff),
    _creationTime(them._creationTime),
    networkState(them.networkState),
    coinbase(them.coinbase),
    transactions(them.transactions),
    merkleBranch(them.merkleBranch),
    hasher(them.hasher)
{
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

    uint256_t previousBlockHash;
    memcpy(&previousBlockHash, &this->networkState->previousBlock.raw(), sizeof(previousBlockHash));
    swap32_256(previousBlockHash);

    return json::array({
        int2hex(this->id()),
        Hash256(previousBlockHash).bytes().asHex(),
        this->coinbase->part1().asHex(),
        this->coinbase->part2().asHex(),
        this->merkleBranch->asHexArray(),
        int2hex(this->networkState->version),
        int2hex(this->networkState->bits, true),
        int2hex(this->creationTime()),
        force
    });
}

bool StratumJob::checkSolution(uint32_t time, uint32_t nonce, CoinbaseTransaction::nonce1_t extraNonce1, CoinbaseTransaction::nonce2_t extraNonce2, BlockSubmitter &blockSubmitter, const char *logId)
{
    static const HashPluginRef sha256dHasher = get_hashplugin("sha256d");

    ByteString rawCoinbaseTx = this->coinbase->part1();

    rawCoinbaseTx.appendLE(extraNonce1);
    rawCoinbaseTx.appendLE(extraNonce2);

    rawCoinbaseTx += this->coinbase->part2();

    Hash256 coinbaseTxId = sha256dHasher->hash(rawCoinbaseTx);
    MerkleRoot merkleRoot = this->merkleBranch->getRoot(coinbaseTxId);
    BlockHeader header(this->networkState->version, this->networkState->previousBlock, merkleRoot, time, this->networkState->bits, nonce);
    BlockHash hash = header.hash(this->hasher);

    mlog(DEBUG, "[%s] Received solution from miner: time %08x, nonce %08x, extraNonce2 %08x, share %s", logId, time, nonce, extraNonce2, hash.bytes().asHex().c_str());

    bool submit = hash <= this->networkState->miningTarget;

    /* Async */
    if (submit)
        blockSubmitter.submitBlock(header, rawCoinbaseTx, this->transactions);

    double shareDiff = Hash256::diff1() / hash / 0x1000;
    double shareMiningDiff = shareDiff * 0x100;

    mlog(DEBUG, "[%s] Share %s, diff %.3f (%.3f/%.3f)", logId, hash.bytes().asHex().c_str(), shareDiff, shareMiningDiff, this->diff());

    if (shareMiningDiff < this->diff())
    {
        mlog(WARNING, "[%s] Client sent low diff share: %.3f < %.3f", logId, shareMiningDiff, this->diff());
        throw validation_error("low diff");
    }

#ifdef SHARE_TARGET_DEBUG
    mlog(DEBUG, "[%s] Share  %s %s below", logId, hash.bytes().asHex().c_str(), submit ? "WAS" : "was not");
    mlog(DEBUG, "[%s] target %s%s", logId, this->networkState->miningTarget.bytes().asHex().c_str(), submit ? ", SUBMITTED!" : "");
#endif

    return submit;
}
