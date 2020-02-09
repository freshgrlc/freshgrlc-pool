#include "ByteString.h"

#include <cstring>

static uint8_t _toRaw(char hex)
{
    if (hex >= '0' && hex <= '9')
        return hex - '0';

    if (hex >= 'a' && hex <= 'f')
        return hex - 'a' + 10;

    if (hex >= 'A' && hex <= 'F')
        return hex - 'A' + 10;

    return 0;
}

static uint8_t toRaw(const char *hex)
{
    return (_toRaw(hex[0]) << 4) | _toRaw(hex[1]);
}

ByteString &ByteString::operator+=(const ConstByteStringRef &toAppend)
{
    size_t oldsize = this->size();

    this->resize(oldsize + toAppend.length);

    for (size_t i = 0; i < toAppend.length; i++)
        (*this)[oldsize + i] = toAppend.data[i];

    return *this;
}

ByteString &ByteString::operator+=(const ByteString &toAppend)
{
    if (&toAppend != this)
        return *this += ConstByteStringRef(toAppend);

    size_t size = this->size();

    this->resize(size * 2);
    memcpy(this->data() + size, this->data(), size);

    return *this;
}

ByteString ByteString::reverse()
{
    std::vector<uint8_t> reversed(this->size());

    for (size_t i = 0; i < this->size(); i++)
        reversed[i] = (*this)[this->size() - 1 - i];

    return ByteString(reversed);
}

ByteString ByteString::fromHex(const std::string &hex)
{
    uint8_t buffer[(hex.length() + 1) / 2];
    uint8_t *ptr = buffer;
    size_type pos = 0;

    if ((hex.length() % 2))
        *ptr++ = _toRaw(hex[pos++]);

    for (; pos < hex.length(); pos += 2)
        *ptr++ = toRaw(&hex[pos]);

    return ByteString(buffer, sizeof(buffer));
}
