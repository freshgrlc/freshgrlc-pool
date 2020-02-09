#include "BlockHeader.h"

static uint32_t readBE(uint8_t int32[4])
{
    return (int32[0] << 24) | (int32[1] << 16) | (int32[2] << 8) | int32[3];
}

static uint32_t swap32(uint32_t x)
{
    return readBE((uint8_t *) &x);
}

static void swap32(uint8_t int32[4])
{
    *((uint32_t *) int32) = readBE(int32);
}

BlockHeader::BlockHeader(uint32_t versionBits, const Hash256 &parentBlockhash, const Hash256 &merkleRoot, uint32_t time, uint32_t difficultyBits, uint32_t nonce)
{
    this->raw.versionBits = versionBits;
    this->raw.timestamp = time;
    this->raw.difficultyBits = swap32(difficultyBits);
    this->raw.nonce = nonce;

    memcpy(&this->raw.parentBlockHash, ByteString(parentBlockhash.bytes()).reverse().data(), sizeof(this->raw.parentBlockHash));
    memcpy(&this->raw.merkleRootHash, &merkleRoot.raw, sizeof(this->raw.merkleRootHash));

    for (size_t i = 0; i < sizeof(this->raw.parentBlockHash); i += 4)
        swap32(&this->raw.parentBlockHash[i]);
}

BlockHeader::BlockHeader(const ConstByteStringRef &raw)
{
    if (raw.length != sizeof(this->raw))
        throw std::runtime_error("Cannot parse block header from raw byte string: Invalid size " + std::to_string(raw.length));

    memcpy(&this->raw, raw.data, sizeof(this->raw));
}

BlockHash BlockHeader::hash(const HashPluginRef &hasher)
{
    return hasher->hash(ConstByteStringRef((const uint8_t *) &this->raw, sizeof(this->raw)));
}
