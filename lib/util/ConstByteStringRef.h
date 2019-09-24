#ifndef UTIL_CONSTBYTESTRINGREF_H
#define UTIL_CONSTBYTESTRINGREF_H

#include <stdint.h>
#include <vector>
#include <string>

class ConstByteStringRef
{
    public:
        const uint8_t * const data;
        const size_t length;

        inline ConstByteStringRef(const uint8_t *data, size_t length) :
            data(data),
            length(length)
        {}

        inline ConstByteStringRef(const std::vector<uint8_t> &v) :
            data(&v.begin()[0]),
            length(v.size())
        {}

        inline ConstByteStringRef(const std::string &s) :
            data((uint8_t *) s.c_str()),
            length(s.length())
        {}

        template<size_t N>
        inline ConstByteStringRef(const uint8_t (&data)[N]) :
            data(data),
            length(N)
        {}

        inline const uint8_t &operator[](int pos) const     { return this->data[pos]; }

        inline std::string asString(void) const             { return std::string((const char *) this->data, this->length); }
        inline std::string asHex(void) const                { return this->asHexPretty("", "", -1); }

        std::string asHexPretty(const std::string prefix = "<", const std::string postfix = ">", int groupSize = 4) const;

        template <class T>
        inline static ConstByteStringRef raw(const T &v)    { return ConstByteStringRef((const uint8_t *) &v, sizeof(T)); }

};

#endif
