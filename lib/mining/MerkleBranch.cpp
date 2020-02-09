#include "MerkleBranch.h"

std::vector<std::string> MerkleBranch::asHexArray()
{
    std::vector<std::string> hexArray;

    hexArray.reserve(this->size());

    for (Hash256 &node : *this)
        hexArray.push_back(node.bytes().asHex());

    return hexArray;
}
