#ifndef ALLIUMHASHPLUGIN_H
#define ALLIUMHASHPLUGIN_H

#include <interfaces/plugins/HashPlugin.h>

class AlliumHashPlugin : public HashPlugin
{
    public:
        void hash(const uint8_t *data, int length, HashPlugin::uint256_t hash) const override;
};

#endif
