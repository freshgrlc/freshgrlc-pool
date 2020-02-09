#include "MerkleBranch.h"

#include "hashplugin.h"

std::vector<std::string> MerkleBranch::asHexArray()
{
    std::vector<std::string> hexArray;

    hexArray.reserve(this->size());

    for (Hash256 &node : *this)
        hexArray.push_back(node.bytes().asHex());

    return hexArray;
}

MerkleRoot MerkleBranch::getRoot(const Hash256 &leaf)
{
    return MerkleRoot(this->hashRecursive(leaf, this->begin()));
}

MerkleNode MerkleBranch::hashRecursive(const MerkleNode &hash, iterator it)
{
    static const HashPluginRef hasher = get_hashplugin("sha256d");

    if (it == this->end())
        return hash;

    return this->hashRecursive(hasher->hash(ByteString() + hash + it[0]), it + 1);
}

