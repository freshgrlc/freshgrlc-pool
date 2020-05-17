#ifndef STRATUM_POOL_H
#define STRATUM_POOL_H

#include "StratumConnectionManager.h"
#include "JobManager.h"
#include "BlockGenerationInformation.h"

#include <util/WorkerThread.h>


class StratumServer;

class Pool
{
    public:
        class WorkUpdate
        {
            public:
                const BlockGenerationInformationRef informationUpdate;
                inline WorkUpdate(BlockGenerationInformationRef informationUpdate) : informationUpdate(informationUpdate) {}
        };

        Pool(StratumServer &server, double defaultDiff = 1.0);

        void updateWork(WorkUpdate update);

        inline StratumServer &server(void)      { return _server; }

    private:
        class WorkUpdater : public WorkerThread<WorkUpdate>
        {
            public:
                WorkUpdater(Pool &parent);

            private:
                Pool &parent;

                void processEvent(const WorkerThread::EventRef &event) override;
        };

        StratumServer &_server;

        StratumConnectionManager connectionManager;
        JobManager jobManager;
        Queue<WorkUpdate> workUpdateQueue;
        WorkUpdater workUpdater;
};

#endif
