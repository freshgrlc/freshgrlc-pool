#include "MerkleRoot.h"
#include "hashplugin.h"
#include "block.h"

static const HashPluginRef hasher = get_hashplugin("sha256d");

MerkleNode MerkleNode::fromRawTxIdBlob(const ConstByteStringRef &txids)
{
    int size = txids.length / sizeof(uint256_t);

    if ((txids.length % sizeof(uint256_t)))
        throw std::runtime_error("Invalid input: raw txids array not divisible by transaction id size");

    switch (size)
    {
        case 1:
            return MerkleNode(txids);

        case 2:
            return hasher->hash(txids);

        default:
            size_t chunksize = sizeof(uint256_t) * 2;
            ByteString row = ByteString::empty();

            for (size_t processed = 0; processed < txids.length; processed += chunksize)
            {
                if (txids.length - processed >= chunksize)
                    row += fromRawTxIdBlob(ConstByteStringRef(txids.data + processed, chunksize));
                else
                {
                    /* Only 1 ID left, so copy it and hash against itself */
                    ByteString finalInput = ByteString(txids.data + processed, sizeof(uint256_t));

                    printf("input (1): %s\n", finalInput.asHex().c_str());

                    finalInput += finalInput;

                    printf("input (2): %s\n", finalInput.asHex().c_str());

                    row += fromRawTxIdBlob(finalInput);
                }
            }

            return fromRawTxIdBlob(row);
    }
}

MerkleRoot MerkleRoot::fromTxIds(const std::vector<Hash256> &txs)
{
    ByteString rawBuffer = ByteString::empty();

    for (auto &txid : txs)
        rawBuffer += txid;

    return fromRawTxIdBlob(rawBuffer);
}
