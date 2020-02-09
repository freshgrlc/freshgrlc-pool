#ifndef FRESHGRLC_POOL_HASHPLUGIN_H
#define FRESHGRLC_POOL_HASHPLUGIN_H

#include <stdint.h>
#include <memory>

#include <mining/Hash256.h>

#include <util/ByteString.h>


class HashPlugin
{
    public:
        inline virtual ~HashPlugin(void) {}

        typedef uint8_t uint256_t[32];
        virtual void hash(const uint8_t *data, int length, uint256_t hash) const = 0;

        inline Hash256 hash(const ConstByteStringRef &data) const
        {
            uint256_t hash;

            this->hash(data.data, data.length, hash);
            return hash;
        }
};

typedef std::shared_ptr<const HashPlugin> HashPluginRef;


extern "C" const HashPlugin *initialize_plugin(void);

typedef const HashPlugin *(*HASHPLUGIN_INITIALIZER)(void);
#define HASHPLUGIN_INITIALIZER_NAME "initialize_plugin"

#endif
