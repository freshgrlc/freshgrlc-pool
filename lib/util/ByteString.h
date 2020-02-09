#ifndef UTIL_BYTESTRING_H
#define UTIL_BYTESTRING_H

#include <stdint.h>
#include <vector>
#include <string>

#include "ConstByteStringRef.h"


class ByteString : public std::vector<uint8_t>
{
    public:
        inline ByteString(ByteString &&src) : vector(src) {}

        inline ByteString(void) : vector() {}
        inline ByteString(const uint8_t *data, int length) : vector(data, data + length) {}
        inline ByteString(const ConstByteStringRef &data) : vector(data.data, data.data + data.length) {}
        inline ByteString(const std::string &s) : vector((uint8_t *) s.c_str(), (uint8_t *) s.c_str() + s.length()) {}
        inline ByteString(const ByteString &data) : vector(data.begin(), data.end()) {}

        template<size_t N>
        inline ByteString(const uint8_t (&data)[N]) : vector(data, data + N) {}

        ByteString &operator+=(const ConstByteStringRef &toAppend);
        ByteString &operator+=(const ByteString &toAppend);
        inline ByteString &operator+=(uint8_t b)                        { this->push_back(b); return *this; }

        inline ByteString operator+(const ConstByteStringRef &toAppend) { ByteString copy(*this); copy += toAppend; return copy; }
        inline ByteString operator+(const ByteString &toAppend)         { return *this + ConstByteStringRef(toAppend); }

        inline ByteString &appendLE(uint16_t num)                       { return *this += ConstByteStringRef((uint8_t *) &num, sizeof(num)); }
        inline ByteString &appendLE(uint32_t num)                       { return *this += ConstByteStringRef((uint8_t *) &num, sizeof(num)); }
        inline ByteString &appendLE(uint64_t num)                       { return *this += ConstByteStringRef((uint8_t *) &num, sizeof(num)); }

        ByteString reverse(void);

        inline std::string asString(void) const                         { return ConstByteStringRef(*this).asString(); }
        inline std::string asHex(void) const                            { return ConstByteStringRef(*this).asHex(); }
        inline std::string asHexPretty(const std::string prefix = "<", const std::string postfix = ">", int groupSize = 4) const
                                                                        { return ConstByteStringRef(*this).asHexPretty(prefix, postfix, groupSize); }

        static ByteString fromHex(const std::string &hex);

        inline static ByteString empty(void)                            { return ByteString(NULL, 0); }
};

#endif
