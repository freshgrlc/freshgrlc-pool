#ifndef STRATUMCONFIGURATION_H
#define STRATUMCONFIGURATION_H

#include <string>

class StratumConfiguration
{
    public:
        inline StratumConfiguration(int port, const std::string &hashingAlgorithm, const std::string &coinbaseSignature, double initialDifficulty = 1.0) :
            port(port),
            hashingAlgorithm(hashingAlgorithm),
            coinbaseSignature(coinbaseSignature),
            initialDifficulty(initialDifficulty)
        {}

        const int port;
        const std::string hashingAlgorithm;
        const std::string coinbaseSignature;
        const double initialDifficulty;
};

#endif
