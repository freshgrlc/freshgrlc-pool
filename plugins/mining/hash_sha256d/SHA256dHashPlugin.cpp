#include "SHA256dHashPlugin.h"
#include "external/sha256.h"


void SHA256dHashPlugin::hash(const uint8_t *data, int length, HashPlugin::uint256_t hash) const
{
    CSHA256 hasher;

    hasher.Write(data, length);
    hasher.Finalize((unsigned char *) hash);

    hasher.Reset();
    hasher.Write(hash, sizeof(uint256_t));
    hasher.Finalize((unsigned char *) hash);
}

extern "C" const HashPlugin *initialize_plugin(void)
{
    return new SHA256dHashPlugin();
}
