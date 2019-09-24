#ifndef STRATUM_STRATUMJOB_H
#define STRATUM_STRATUMJOB_H

#include <string>


class StratumJob
{
    public:
        inline StratumJob(uint32_t id, double diff) :
            _id(id),
            _diff(diff)
        {}

        inline uint32_t id(void) const { return _id; }
        inline double diff(void) const { return _diff; }

    private:
        uint32_t _id;
        double _diff;
};

#endif
