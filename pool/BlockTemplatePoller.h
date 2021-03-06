#ifndef BLOCKTEMPLATEPOLLER_H
#define BLOCKTEMPLATEPOLLER_H

#include <bitcoinrpc/BlockTemplate.h>
#include <mining/Hash256.h>

#include <util/Thread.h>


class DaemonConnector;

class BlockTemplatePoller : public Thread
{
    public:
        inline BlockTemplatePoller(DaemonConnector &parent) : Thread(),
            parent(parent),
            lastHeight(0),
            lastPrevBlockHash(Hash256::zero()),
            lastPrevPrevBlockHash(Hash256::zero())
        {}

    private:
        DaemonConnector &parent;

        uint32_t lastHeight;
        Hash256 lastPrevBlockHash;
        Hash256 lastPrevPrevBlockHash;

        void main(void) override;

        void poll(void);
        void updateNetworkState(const BlockTemplate &blockTemplate);
};

#endif
