#ifndef UTIL_BYTESTRING_H
#define UTIL_BYTESTRING_H

#include <stdint.h>
#include <vector>
#include <string>

#include "ConstByteStringRef.h"


class ByteString : public std::vector<uint8_t>
{
    public:
        inline ByteString(const uint8_t *data, int length) : vector(data, data + length) {}
        inline ByteString(const std::string &s) : vector((uint8_t *) s.c_str(), (uint8_t *) s.c_str() + s.length()) {}

        template<size_t N>
        inline ByteString(const uint8_t (&data)[N]) : vector(data, data + N) {}

        inline std::string asString(void) const             { return ConstByteStringRef(*this).asString(); }
        inline std::string asHex(void) const                { return ConstByteStringRef(*this).asHex(); }
        inline std::string asHexPretty(const std::string prefix = "<", const std::string postfix = ">", int groupSize = 4) const
                                                            { return ConstByteStringRef(*this).asHexPretty(prefix, postfix, groupSize); }

        static ByteString fromHex(const std::string &hex);

        inline static ByteString empty(void)                { return ByteString(NULL, 0); }
};

#endif
