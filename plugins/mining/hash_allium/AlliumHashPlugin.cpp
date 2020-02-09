#include <stdexcept>

#include "AlliumHashPlugin.h"
#include "allium_hash.h"

void AlliumHashPlugin::hash(const uint8_t *data, int length, HashPlugin::uint256_t hash) const
{
    if (length != 80)
        throw std::runtime_error("Allium hashing only implemented for exactly 80 bytes of inputdata");

    allium_hash((const char *) data, (char *) hash);
}

extern "C" const HashPlugin *initialize_plugin(void)
{
    return new AlliumHashPlugin();
}

