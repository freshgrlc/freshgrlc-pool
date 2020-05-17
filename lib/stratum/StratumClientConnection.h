#ifndef STRATUM_STRATUMCLIENT_H
#define STRATUM_STRATUMCLIENT_H

#include <socket/IncomingConnection.h>
#include <socket/ConnectionManager.h>
#include <mining/CoinbaseTransaction.h>

#include "StratumCall.h"

#include <util/Lock.h>
#include <util/ByteString.h>

class StratumServer;
class StratumJob;
class JobManager;

class StratumClientConnection : public IncomingConnection
{
    public:
        typedef CoinbaseTransaction::nonce1_t nonce1_t;

        StratumClientConnection(StratumServer &server, SocketBase &&socket);
        virtual ~StratumClientConnection(void);

        void sendReply(const StratumCall &call, const json &reply);
        void sendReply(const StratumCall &call, const json &reply, const json &error);
        void sendUnsolicited(std::string method, const json &params, int id = -1);

        void sendJob(bool forceNew = false);
        void updateDiff(double newDiff);
        inline void acceptedShare(double diff)      { this->acceptedShares += diff; }
        inline double diff(void)                    { return this->currentDiff; }

        void send(const json &payload);
        using IncomingConnection::send;

        const StratumJob &job(bool forceNew = false);
        void clearJobs(void);

        std::unique_ptr<StratumJob> getActiveJob(uint32_t id);

        inline std::string connectionId(void) const { return _connectionId.asHex(); }
        inline const char *logId(void) const        { return _logId.c_str(); }
        inline nonce1_t extraNonce1(void) const     { return *((const nonce1_t *) &_connectionId.begin()[0]); }
        inline StratumServer &server(void)          { return _server; }

        JobManager &jobManager(bool exceptionOnNull = true) const;

    private:
        StratumServer &_server;
        std::string clientSoftware;
        std::string clientUsername;
        ByteString _connectionId;
        double currentDiff;
        time_t startedMining;
        double acceptedShares;
        std::string _logId;

        std::vector<std::unique_ptr<StratumJob>> jobs;
        StratumJob *activeJob;
        JobManager *_jobManager;
        Lock _jobsLock;

        ByteString genConnectionID(void) const;
        double calcNewDiff(void);

        virtual void onReceive(const Packet &packet) override;
};

#endif
