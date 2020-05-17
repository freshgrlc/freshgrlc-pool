#ifndef MINING_MERKLEROOT_H
#define MINING_MERKLEROOT_H

#include "Hash256.h"

class MerkleBranch;

class MerkleNode : public Hash256
{
    public:
        inline MerkleNode(const MerkleNode &that) : Hash256(that) {}

    protected:
        friend class MerkleBranch;

        inline MerkleNode(const Hash256 raw) : Hash256(raw) {}

        static MerkleNode fromRawTxIdBlob(const ConstByteStringRef &txids);
};

class MerkleRoot : public MerkleNode
{
    public:
        static MerkleRoot fromTxIds(const std::vector<Hash256> &txs);

    private:
        friend class MerkleBranch;

        inline MerkleRoot(const MerkleNode &rootNode) : MerkleNode(rootNode) {}
};

#endif
