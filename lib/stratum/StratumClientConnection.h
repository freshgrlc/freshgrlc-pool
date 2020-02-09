#ifndef STRATUM_STRATUMCLIENT_H
#define STRATUM_STRATUMCLIENT_H

#include <socket/ConnectionManager.h>

#include "StratumCall.h"

#include <util/Lock.h>
#include <util/ByteString.h>


class StratumServer;
class StratumJob;

class StratumClientConnection : public ConnectionManager::Connection
{
    public:
        StratumClientConnection(ConnectionManager &manager, SocketBase &&socket, StratumServer *server);
        virtual ~StratumClientConnection(void);

        void sendReply(const StratumCall &call, const json &reply);
        void sendReply(const StratumCall &call, const json &reply, const json &error);
        void sendUnsolicited(std::string method, const json &params, int id = -1);

        void sendJob(bool forceNew = false);
        void updateDiff(double newDiff);
        inline double diff(void)                    { return this->currentDiff; }

        void send(const json &payload);
        using Connection::send;

        const StratumJob &job(bool forceNew = false);
        void clearJobs(void);

        inline std::string connectionId(void) const { return _connectionId.asHex(); }
        inline uint32_t extraNonce1(void) const     { return *((const uint32_t *) &_connectionId.begin()[0]); }
        inline StratumServer &server(void)          { return _server; }

    private:
        StratumServer &_server;
        std::string clientSoftware;
        std::string clientUsername;
        ByteString _connectionId;
        double currentDiff;

        std::vector<std::unique_ptr<StratumJob>> jobs;
        StratumJob *activeJob;
        Lock _jobsLock;

        ByteString genConnectionID(void) const;

        virtual void onReceive(const Packet &packet) override;
};

#endif
