#include "ConstByteStringRef.h"

std::string ConstByteStringRef::asHexPretty(const std::string prefix, const std::string postfix, int groupSize, char delimiter) const
{
    std::string hex = prefix;
    char b[3];

    for (size_t pos = 0; pos < this->length; pos++)
    {
        if (groupSize > 0 && pos && !(pos % groupSize))
            hex += delimiter;

        sprintf(b, "%02x", (*this)[pos]);
        hex += b;
    }

    return hex + postfix;
}
