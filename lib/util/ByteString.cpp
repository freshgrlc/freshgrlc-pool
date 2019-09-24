#include "ByteString.h"

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
