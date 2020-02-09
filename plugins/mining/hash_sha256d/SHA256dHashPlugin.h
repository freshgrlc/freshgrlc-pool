#ifndef SHA256DHASHPLUGIN_H
#define SHA256DHASHPLUGIN_H

#include <interfaces/plugins/HashPlugin.h>

class SHA256dHashPlugin : public HashPlugin
{
    public:
        void hash(const uint8_t *data, int length, HashPlugin::uint256_t hash) const override;
};

#endif
