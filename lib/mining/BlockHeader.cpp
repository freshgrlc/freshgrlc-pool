#include "BlockHeader.h"

#include "swap32.h"


BlockHeader::BlockHeader(uint32_t versionBits, const Hash256 &parentBlockhash, const Hash256 &merkleRoot, uint32_t time, uint32_t difficultyBits, uint32_t nonce)
{
    this->raw.versionBits = versionBits;
    this->raw.timestamp = time;
    this->raw.difficultyBits = swap32(difficultyBits);
    this->raw.nonce = nonce;

    memcpy(&this->raw.parentBlockHash, &parentBlockhash.raw, sizeof(this->raw.parentBlockHash));
    memcpy(&this->raw.merkleRootHash, &merkleRoot.raw, sizeof(this->raw.merkleRootHash));
}

BlockHeader::BlockHeader(const ConstByteStringRef &raw)
{
    if (raw.length != sizeof(this->raw))
        throw std::runtime_error("Cannot parse block header from raw byte string: Invalid size " + std::to_string(raw.length));

    memcpy(&this->raw, raw.data, sizeof(this->raw));
}

BlockHash BlockHeader::hash(const HashPluginRef &hasher) const
{
    Hash256 reversedHash = hasher->hash(ConstByteStringRef((const uint8_t *) &this->raw, sizeof(this->raw)));
    return (Hash256) ByteString(reversedHash.bytes()).reverse();
}

BlockHeader::operator const ByteString() const
{
    return ConstByteStringRef((uint8_t *) &this->raw, sizeof(this->raw));
}
