#ifndef BITCOINRPC_BLOCKCHAININFO_H
#define BITCOINRPC_BLOCKCHAININFO_H

#include <mining/Hash256.h>

class BlockChainInfo
{
    public:
        enum ChainType
        {
            Main,
            Testnet,
            RegTest,

            Unknown
        };

        const ChainType chain;
        const int height;
        const bool synced;
        const Hash256 chaintip;

        inline BlockChainInfo(ChainType chain, int height, bool synced, const Hash256 &chaintip) :
            chain(chain),
            height(height),
            synced(synced),
            chaintip(chaintip)
        {}
};

#endif
