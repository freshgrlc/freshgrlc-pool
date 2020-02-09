#ifndef MINING_MERKLEBRANCH_H
#define MINING_MERKLEBRANCH_H

#include <vector>

#include <mining/MerkleRoot.h>

class MerkleBranch : public std::vector<MerkleNode>
{
    public:
        std::vector<std::string> asHexArray(void);
};

#include <memory>

typedef std::shared_ptr<MerkleBranch> MerkleBranchRef;

#endif
