#ifndef MINING_RAWTRANSACTIONS_H
#define MINING_RAWTRANSACTIONS_H

#include "RawTransaction.h"

#include <vector>

class RawTransactions : public std::vector<RawTransaction>
{
    public:
        using vector::vector;

        operator ByteString(void);
};


#include <memory>

typedef std::shared_ptr<RawTransactions> RawTransactionsRef;

#endif
