#ifndef MINING_MERKLEBRANCH_H
#define MINING_MERKLEBRANCH_H

#include <vector>

#include <mining/MerkleRoot.h>

class MerkleBranch : public std::vector<MerkleNode>
{
    public:
        std::vector<std::string> asHexArray(void);

        MerkleRoot getRoot(const Hash256 &leaf);

    private:
        MerkleNode hashRecursive(const MerkleNode &hash, iterator it);
};

#include <memory>

typedef std::shared_ptr<MerkleBranch> MerkleBranchRef;

#endif
