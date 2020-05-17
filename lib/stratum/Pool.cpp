#include "Pool.h"


Pool::Pool(StratumServer &server, double defaultDiff) :
    _server(server),
    connectionManager(*this),
    jobManager(server, defaultDiff),
    workUpdater(*this)
{
}

void Pool::updateWork(WorkUpdate update)
{
    this->workUpdateQueue.queue(std::move(update));
}

Pool::WorkUpdater::WorkUpdater(Pool &parent) : WorkerThread<WorkUpdate>(parent.workUpdateQueue),
    parent(parent)
{
}

void Pool::WorkUpdater::processEvent(const WorkerThread<WorkUpdate>::EventRef &event)
{
    this->parent.jobManager.updateWork(event->informationUpdate);
}
