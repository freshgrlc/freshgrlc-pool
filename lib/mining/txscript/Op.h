#ifndef MINING_TXSCRIPT_OP_H
#define MINING_TXSCRIPT_OP_H

#include <util/ByteString.h>


namespace script {

enum OP_CODES
{
    OP_0            = 0x00,

    OP_PUSHDATA1    = 0x4c,
    OP_PUSHDATA2    = 0x4d,
    OP_PUSHDATA4    = 0x4e,

    OP_1            = 0x51,
    OP_2            = 0x52,
    OP_3            = 0x53,
    OP_4            = 0x54,
    OP_5            = 0x55,
    OP_6            = 0x56,
    OP_7            = 0x57,
    OP_8            = 0x58,
    OP_9            = 0x59,
    OP_10           = 0x5a,
    OP_11           = 0x5b,
    OP_12           = 0x5c,
    OP_13           = 0x5d,
    OP_14           = 0x5e,
    OP_15           = 0x5f,
    OP_16           = 0x60,

    OP_RETURN       = 0x6a,

    OP_DUP          = 0x76,

    OP_EQUAL        = 0x87,
    OP_EQUALVERIFY  = 0x88,

    OP_HASH160      = 0xa9,
    OP_CHECKSIG     = 0xac,

    OP_FALSE        = OP_0,
    OP_TRUE         = OP_1
};

#define OP_PUSHDATA_LITERAL_START   0x01
#define OP_PUSHDATA_LITERAL_LAST    0x10

#define OP_PUSHDATA_LITERAL_OFFSET  (OP_1 - OP_PUSHDATA_LITERAL_START)

#define OP_PUSHDATA_DIRECT_START    0x01
#define OP_PUSHDATA_DIRECT_LAST     0x4b


class Op
{
    public:
        virtual ByteString raw(void) const = 0;

        inline operator const ByteString(void) const { return this->raw(); }
};

};

#endif
