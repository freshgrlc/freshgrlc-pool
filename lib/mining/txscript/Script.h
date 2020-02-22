#ifndef MINING_TXSCRIPT_SCRIPT_H
#define MINING_TXSCRIPT_SCRIPT_H

#include <util/ByteString.h>

#include "Op.h"
#include "PushData.h"

#include <string.h>

namespace script {

class Script : public ByteString
{
    public:
        using ByteString::ByteString;
        using ByteString::operator+=;

        inline ByteString &operator+=(OP_CODES op)                      { return *this += (uint8_t) op; }

        inline ByteString &operator+=(const PushData &data)             { return this->ByteString::operator+=(data); }
        inline ByteString &operator+=(const ByteString &data)           { return this->operator+=(PushData(data)); }
        inline ByteString &operator+=(const ConstByteStringRef &data)   { return this->operator+=(PushData(data)); }

        template <class op_type>
        inline Script &operator<<(op_type data)                         { return *((Script *) &(*this += data)); }

        inline Script &operator=(const Script &src)
        {
            if (this != &src)
            {
                this->clear();
                this->resize(src.size());

                memcpy(&(*this)[0], &src[0], src.size());
            }

            return *this;
        }
};

};

#endif
