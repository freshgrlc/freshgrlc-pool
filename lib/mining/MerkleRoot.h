#ifndef MINING_MERKLEROOT_H
#define MINING_MERKLEROOT_H

#include "Hash256.h"

class MerkleNode : public Hash256
{
    protected:
        inline MerkleNode(const Hash256 raw) : Hash256(raw) {}
        inline MerkleNode(const MerkleNode &that) : Hash256(that) {}

        static MerkleNode fromRawTxIdBlob(const ConstByteStringRef &txids);

    friend class MerkleBranch;
};

class MerkleRoot : public MerkleNode
{
    public:
        static MerkleRoot fromTxIds(const std::vector<Hash256> &txs);

    private:
        inline MerkleRoot(const MerkleNode &rootNode) : MerkleNode(rootNode) {}

    friend class MerkleBranch;
};

#endif
