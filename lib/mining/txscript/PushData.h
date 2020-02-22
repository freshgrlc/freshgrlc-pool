#ifndef MINING_TXSCRIPT_PUSHDATA_H
#define MINING_TXSCRIPT_PUSHDATA_H

#include "Op.h"

namespace script {

class PushData : public Op
{
    public:
        PushData(uint32_t integer);
        PushData(uint64_t integer);
        PushData(const ConstByteStringRef &data);

        ByteString raw() const override;

    private:
        ByteString data;
};

};

#endif
