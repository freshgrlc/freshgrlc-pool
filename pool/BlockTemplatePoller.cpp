#include "BlockTemplatePoller.h"
#include "DaemonConnector.h"

#include <unistd.h>

#define ONE_SECOND  1000000


void BlockTemplatePoller::main()
{
    for (;; usleep(ONE_SECOND))
        this->poll();
}

void BlockTemplatePoller::poll()
{
    bool update = false;
    auto blockTemplate = this->parent.getBlockTemplate();

    if (blockTemplate.height != this->lastHeight)
    {
        if (blockTemplate.height == this->lastHeight - 1 && this->lastPrevPrevBlockHash == blockTemplate.previousBlockHash)
        {
            /* Ignore: Node is lagging behind */
        }
        else
            update = true;
    }
    else
    {
        /*
         *  In case of a chainsplit different nodes might report different
         *  hashes for the same height.
         *
         *  This should typically be ignored, unless the parent of both
         *  chaintips is different as well (which might indicate a reorder)
         *
         *  TODO: Check lastPrevPrevBlockHash against current tip's parent.
         */
    }

    if (update)
    {
        if (this->lastHeight == blockTemplate.height - 1)
            // FIXME: Check if this is indeed the case
            this->lastPrevPrevBlockHash = this->lastPrevBlockHash;
        else
            this->lastPrevPrevBlockHash = Hash256::zero();

        this->lastHeight = blockTemplate.height;
        this->lastPrevBlockHash = blockTemplate.previousBlockHash;

        this->updateNetworkState(blockTemplate);
    }
}

void BlockTemplatePoller::updateNetworkState(const BlockTemplate &blockTemplate)
{
    this->parent.updateStratumServer(this->parent.getNetworkState(blockTemplate));
}
