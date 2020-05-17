#include "MerkleBranch.h"

#include "hashplugin.h"

std::vector<std::string> MerkleBranch::asHexArray() const
{
    std::vector<std::string> hexArray;

    hexArray.reserve(this->size());

    for (const Hash256 &node : *this)
        hexArray.push_back(node.bytes().asHex());

    return hexArray;
}

MerkleRoot MerkleBranch::getRoot(const Hash256 &leaf) const
{
    return MerkleRoot(this->hashRecursive(leaf, this->begin()));
}

MerkleNode MerkleBranch::hashRecursive(const MerkleNode &hash, const_iterator it) const
{
    static const HashPluginRef hasher = get_hashplugin("sha256d");

    if (it == this->end())
        return hash;

    return this->hashRecursive(hasher->hash(ByteString() + hash + it[0]), it + 1);
}

