#include <nlohmann/json.hpp>

#include "StratumClientConnection.h"

#include "StratumCall.h"
#include "StratumServer.h"
#include "StratumJob.h"
#include "calls/MiningSubscribe.h"
#include "calls/MiningAuthorize.h"

#include <util.h>

#include <ctime>
#include <openssl/sha.h>


#define SHARES_PER_MINUTE_TARGET        6
#define SHARES_PER_MINUTE_TARGET_FUZZ   1.1
#define DIFF_FLOOR                      0.01


using json_exception = nlohmann::detail::exception;


StratumClientConnection::StratumClientConnection(ConnectionManager &manager, SocketBase &&socket, StratumServer *server) : Connection(manager, std::move(socket)),
    _server(*server),
    clientSoftware("<unknown mining software>"),
    clientUsername(""),
    _connectionId(this->genConnectionID()),
    currentDiff(-1.0),
    startedMining(std::time(NULL)),
    acceptedShares(0.0),
    activeJob(nullptr)
{
    mlog(INFO, "New stratum connection %s", this->connectionId().c_str());
    this->_logId = this->connectionId().substr(0, 8);
}

StratumClientConnection::~StratumClientConnection()
{
    mlog(INFO, "Closing connection %s (user '%s')", this->connectionId().c_str(), this->clientUsername.c_str());
}

void StratumClientConnection::sendReply(const StratumCall &call, const json &reply)
{
    this->sendReply(call, reply, "null"_json);
}

void StratumClientConnection::sendReply(const StratumCall &call, const json &reply, const json &error)
{
    json replyMessage = {
        {"id", call.messageId},
        {"result", reply},
        {"error", error}
    };

    this->send(replyMessage);
}

void StratumClientConnection::sendUnsolicited(std::string method, const json &params, int id)
{
    json msg = {
        { "id", nullptr },
        { "method", method },
        { "params", params }
    };

    if (id > 0)
        msg["id"] = id;

    this->send(msg);
}

void StratumClientConnection::sendJob(bool forceNew)
{
    auto &job = this->job(forceNew);

    if (job.diff() != this->currentDiff)
        this->updateDiff(job.diff());

    /* Send job */
    this->sendUnsolicited("mining.notify", job.toJson(forceNew));
}

void StratumClientConnection::updateDiff(double newDiff)
{
    mlog(DEBUG, "[%s] Updating client diff to %.3f", this->logId(), newDiff);
    this->sendUnsolicited("mining.set_difficulty", (json) { newDiff });
    this->currentDiff = newDiff;
}

void StratumClientConnection::send(const json &payload)
{
    this->send(Packet(payload.dump() + "\n"));
}

const StratumJob &StratumClientConnection::job(bool forceNew)
{
    OBTAIN_LOCK(_jobsLock);

    if (!forceNew && this->activeJob)
        return *this->activeJob;

    this->jobs.push_back(this->server().createJob(this, this->calcNewDiff()));
    this->activeJob = this->jobs[this->jobs.size()-1].get();

    mlog(DEBUG, "[%s] New job '%d'", this->logId(), this->activeJob->id());

    return *this->activeJob;
}

void StratumClientConnection::clearJobs()
{
    OBTAIN_LOCK(_jobsLock);

    this->jobs.clear();
    this->activeJob = NULL;

    mlog(DEBUG, "[%s] Cleared jobs", this->logId());
}

std::unique_ptr<StratumJob> StratumClientConnection::getActiveJob(uint32_t id)
{
    OBTAIN_LOCK(_jobsLock);

    if (this->activeJob && this->activeJob->id() == id)
        return std::make_unique<StratumJob>(*this->activeJob);

    if (this->jobs.empty())
        return nullptr;

    for (const auto &job : this->jobs)
    {
        if (job->id() == id)
            return std::make_unique<StratumJob>(*job);
    }

    return nullptr;
}

ByteString StratumClientConnection::genConnectionID() const
{
    static const char PREFIX[] = "stratum client";
    time_t tm = time(NULL);
    static int __counter = 0; __counter++;
    auto meptr = this;

    uint8_t m[sizeof(PREFIX) + sizeof(time_t) + sizeof(__counter) + sizeof(meptr)];

    memcpy(m, PREFIX, sizeof(PREFIX));
    memcpy(m + sizeof(PREFIX), &tm, sizeof(tm));
    memcpy(m + sizeof(PREFIX) + sizeof(tm), &__counter, sizeof(__counter));
    memcpy(m + sizeof(PREFIX) + sizeof(tm) + sizeof(__counter), &meptr, sizeof(meptr));

    uint8_t hash[SHA_DIGEST_LENGTH];

    if (SHA1(m, sizeof(m), hash))
        return ByteString(hash, sizeof(hash));

    throw std::runtime_error("Cannot create stratum connection ID: SHA1() failed");
}

double StratumClientConnection::calcNewDiff()
{
    time_t totalMiningTime = std::time(NULL) - this->startedMining;
    double newDiff = this->acceptedShares / totalMiningTime * 60 / SHARES_PER_MINUTE_TARGET;

    if (totalMiningTime <= 0)
        return -1.0;

    if (this->acceptedShares == 0.0)
    {
        /* Restart measurement */
        this->startedMining = std::time(NULL);

        newDiff = this->currentDiff / 10;
        return newDiff >= DIFF_FLOOR ? newDiff : DIFF_FLOOR;
    }

    if (totalMiningTime < 10)
        return -1.0;

    if (newDiff < DIFF_FLOOR)
        newDiff = DIFF_FLOOR;

    mlog(DEBUG, "[%s] Diff calc: %.3f diff over %d secs = %.3f diff/sec", this->logId(), this->acceptedShares, totalMiningTime, this->acceptedShares / totalMiningTime);
    mlog(DEBUG, "[%s]            New target: %.3f, current band: %.3f - %.3f", this->logId(), newDiff, this->currentDiff / SHARES_PER_MINUTE_TARGET_FUZZ, this->currentDiff * SHARES_PER_MINUTE_TARGET_FUZZ);

    if (newDiff > this->currentDiff * SHARES_PER_MINUTE_TARGET_FUZZ ||
        newDiff < this->currentDiff / SHARES_PER_MINUTE_TARGET_FUZZ)
    {
        return newDiff;
    }

    return this->currentDiff;
}

void StratumClientConnection::onReceive(const Packet &packet)
{
    try
    {
        json payload = json::parse(packet.asString());
        std::unique_ptr<StratumCall> call;

        if (!payload.is_object() || !(call = StratumCall::parseMessage(payload)))
        {
            mlog(WARNING, "[%s] Received invalid stratum packet: %s", this->logId(), packet.data);
            return;
        }

        if (call->method == stratum::messages::MiningSubscribe::METHOD)
        {
            const auto &message = *((stratum::messages::MiningSubscribe *) call.get());

            this->clientSoftware = message.client;
            mlog(INFO, "[%s] Client is using '%s'", this->logId(), this->clientSoftware.c_str());
        }
        else if (call->method == stratum::messages::MiningAuthorize::METHOD)
        {
            const auto &message = *((stratum::messages::MiningAuthorize *) call.get());

            this->clientUsername = message.username;
            mlog(INFO, "[%s] Client identified as '%s'", this->logId(), this->clientUsername.c_str());
            this->_logId = this->clientUsername + "/" + this->connectionId().substr(0, 8);
        }

        call->process(*this);
    }
    catch (json_exception &e)
    {
        mlog(WARNING, "[%s] Received invalid stratum packet: %s", this->logId(), packet.data);
        mlog(WARNING, "[%s] Caused by JSON parse error: %s", this->logId(), e.what());
        return;
    }
}
