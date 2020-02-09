#include "PushData.h"

using namespace script;

static ByteString fromUInt(uint64_t integer)
{
    ByteString encoded;
    bool lastSignFlag = false;

    do
    {
        encoded += (uint8_t) integer;
        lastSignFlag = !!(integer & 0x80);
        integer /= 0x100;
    }
    while (integer || lastSignFlag);

    return encoded;
}

PushData::PushData(uint32_t integer) : data(fromUInt(integer))
{
}

PushData::PushData(uint64_t integer) : data(fromUInt(integer))
{
}

PushData::PushData(const ConstByteStringRef &data) : data(data)
{
}

ByteString script::PushData::raw()
{
    ByteString raw;

    size_t len = this->data.size();

    if (!len)
    {
        raw += (uint8_t) OP_0;
        return raw;
    }
    else if (len == 1)
    {
        if (this->data[0] == 0x00)
        {
            raw += (uint8_t) OP_0;
            return raw;
        }
        else if (this->data[0] >= OP_PUSHDATA_LITERAL_START && this->data[0] <= OP_PUSHDATA_LITERAL_LAST)
        {
            raw += (uint8_t) this->data[0] + OP_PUSHDATA_LITERAL_OFFSET;
            return raw;
        }
    }

    if (len >= OP_PUSHDATA_DIRECT_START && len <= OP_PUSHDATA_DIRECT_LAST)
        raw += (uint8_t) len;
    else if (len >= OP_PUSHDATA1 && len < 0x100)
    {
        raw += (uint8_t) OP_PUSHDATA1;
        raw += (uint8_t) len;
    }
    else if (len >= 0x100 && len < 0x10000)
    {
        raw += (uint8_t) OP_PUSHDATA2;
        raw += (uint8_t) len;
        raw += (uint8_t) (len / 0x100);
    }
    else
    {
        /* Truncate if insanely large */
        if (len >= 0x100000000)
            len = 0xffffffff;

        raw += (uint8_t) OP_PUSHDATA4;
        raw += (uint8_t) len;
        raw += (uint8_t) (len / 0x100);
        raw += (uint8_t) (len / 0x10000);
        raw += (uint8_t) (len / 0x1000000);
    }

    raw += ConstByteStringRef(&this->data[0], len);
    return raw;
}


