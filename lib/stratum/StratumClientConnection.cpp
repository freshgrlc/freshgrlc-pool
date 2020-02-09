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


using json_exception = nlohmann::detail::exception;


StratumClientConnection::StratumClientConnection(ConnectionManager &manager, SocketBase &&socket, StratumServer *server) : Connection(manager, std::move(socket)),
    _server(*server),
    clientSoftware("<unknown mining software>"),
    clientUsername(""),
    _connectionId(this->genConnectionID()),
    currentDiff(-1.0),
    activeJob(nullptr)
{
    mlog(INFO, "New stratum connection %s", this->connectionId().c_str());
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

    this->jobs.push_back(this->server().createJob(this));
    this->activeJob = this->jobs[this->jobs.size()-1].get();

    mlog(DEBUG, "New job '%d'", this->activeJob->id());

    return *this->activeJob;
}

void StratumClientConnection::clearJobs()
{
    OBTAIN_LOCK(_jobsLock);

    this->jobs.clear();
    this->activeJob = NULL;

    mlog(DEBUG, "Cleared jobs");
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

void StratumClientConnection::onReceive(const Packet &packet)
{
    try
    {
        json payload = json::parse(packet.asString());
        std::unique_ptr<StratumCall> call;

        if (!payload.is_object() || !(call = StratumCall::parseMessage(payload)))
        {
            mlog(WARNING, "Received invalid stratum packet: %s", packet.data);
            return;
        }

        if (call->method == stratum::messages::MiningSubscribe::METHOD)
        {
            this->clientSoftware = ((stratum::messages::MiningSubscribe *) call.get())->client;
            mlog(INFO, "Client is using '%s'", this->clientSoftware.c_str());
        }
        else if (call->method == stratum::messages::MiningAuthorize::METHOD)
        {
            this->clientUsername = ((stratum::messages::MiningAuthorize *) call.get())->username;
            mlog(INFO, "Client identified as '%s'", this->clientUsername.c_str());
        }

        call->process(*this);
    }
    catch (json_exception &e)
    {
        mlog(WARNING, "Received invalid stratum packet: %s", packet.data);
        mlog(WARNING, "Caused by JSON parse error: %s", e.what());
        return;
    }
}
