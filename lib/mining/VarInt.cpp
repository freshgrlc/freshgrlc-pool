#include "VarInt.h"

ByteString VarInt::encode()
{
    ByteString encoded;

    if (this->raw < 0xfd)
        return ConstByteStringRef((uint8_t *) &this->raw, 1);

    if (this->raw < 0x10000)
    {
        encoded += (uint8_t) 0xfd;
        encoded += ConstByteStringRef((uint8_t *) &this->raw, 2);
    }
    else if (this->raw < 0x100000000)
    {
        encoded += (uint8_t) 0xfe;
        encoded += ConstByteStringRef((uint8_t *) &this->raw, 4);
    }
    else
    {
        encoded += (uint8_t) 0xff;
        encoded += ConstByteStringRef((uint8_t *) &this->raw, 8);
    }

    return encoded;
}
