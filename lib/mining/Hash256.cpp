#include "Hash256.h"
#include "swap32.h"

static int findFirstNonZeroByte(const uint8_t *data, int max)
{
    int idx = 0;

    for (; idx < max && !data[idx]; idx++);

    return idx;
}

double Hash256::operator/(const Hash256 &operand) const
{
    double baseFactor = 1.0;

    int idxMe = findFirstNonZeroByte(this->_raw, sizeof(this->_raw) - sizeof(uint32_t));
    int idxOp = findFirstNonZeroByte(operand._raw, sizeof(operand._raw) - sizeof(uint32_t));
    int idxDiff = idxMe - idxOp;

    while (idxDiff < 0)
    {
        idxDiff++;
        baseFactor *= 256;
    }
    while (idxDiff > 0)
    {
        idxDiff--;
        baseFactor /= 256;
    }

    return (double) readBE32(&this->_raw[idxMe]) / (double) readBE32(&operand._raw[idxOp]) * baseFactor;
}

Hash256 Hash256::diff1()
{
    static uint256_t _diff1 = {
        0x00, 0x00, 0x0f, 0xff,
        0xff, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };

    return Hash256(_diff1);
}
