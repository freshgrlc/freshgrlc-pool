#include "Hash.h"
#include "Hash160.h"
#include "Hash256.h"

#include <string.h>
#include <stdexcept>

template <class hash_t>
Hash<hash_t>::Hash(size_t s) : _raw()
{
    memset((void *) _raw, 0, s);
}

template <class hash_t>
Hash<hash_t>::Hash(const uint8_t *src, size_t s) : _raw()
{
    memcpy((void *) _raw, src, s);
}

template <class hash_t>
Hash<hash_t>::Hash(const ConstByteStringRef &src, size_t s) : _raw()
{
    if (src.length != s)
        throw std::runtime_error("Cannot interpret data of " + std::to_string(src.length) + " bytes as " + std::to_string(s) + " byte hash");

    memcpy((void *) _raw, src.data, s);
}

template <class hash_t>
bool Hash<hash_t>::isEqual(const Hash &them, size_t s) const
{
    return !memcmp(_raw, them._raw, s);
}

template <class hash_t>
bool Hash<hash_t>::isLower(const Hash &them, size_t s) const
{
    return this->isLower(them, false, s);
}

template <class hash_t>
bool Hash<hash_t>::isLower(const Hash &them, bool orEqual, size_t s) const
{
    return memcmp(_raw, them._raw, s) < (orEqual ? 1 : 0);
}

template<typename hash_t>
void Hash<hash_t>::assign(const Hash<hash_t> &src, size_t s)
{
    memcpy(_raw, src._raw, s);
}

Hash160::Hash160(const Hash160 &src) : Hash(src) {}
Hash160::Hash160(const uint160_t raw) : Hash(raw) {}
Hash160::Hash160(const ByteString &src) : Hash((ConstByteStringRef) src) {}
Hash160::Hash160(const ConstByteStringRef &src) : Hash(src) {}
bool Hash160::operator==(const Hash160 &them) const { return this->isEqual(them); }
bool Hash160::operator<(const Hash160 &them) const { return this->isLower(them); }
bool Hash160::operator<=(const Hash160 &them) const { return this->isLower(them), true; }
Hash160 &Hash160::operator=(const Hash160 &src) { this->assign(src); return *this; }
Hash160 Hash160::zero() { return Hash160(); }

Hash256::Hash256(const Hash256 &src) : Hash(src) {}
Hash256::Hash256(const uint256_t raw) : Hash(raw) {}
Hash256::Hash256(const ByteString &src) : Hash((ConstByteStringRef) src) {}
Hash256::Hash256(const ConstByteStringRef &src) : Hash(src) {}
bool Hash256::operator==(const Hash256 &them) const { return this->isEqual(them); }
bool Hash256::operator<(const Hash256 &them) const { return this->isLower(them); }
bool Hash256::operator<=(const Hash256 &them) const { return this->isLower(them, true); }
Hash256 &Hash256::operator=(const Hash256 &src) { this->assign(src); return *this; }
Hash256 Hash256::zero() { return Hash256(); }
