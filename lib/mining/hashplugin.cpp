
#include <interfaces/plugins/HashPlugin.h>

#include <string>
#include <map>
#include <memory>
#include <stdexcept>

#include <dlfcn.h>

#include <util/Lock.h>
#include <util/logger.h>

#undef mlog
#define mlog(level, ...)    _mlog("plugins", level, __VA_ARGS__)


class HashPluginManager : private std::map<std::string, HashPluginRef>
{
    public:
        static HashPluginManager Manager;

        HashPluginRef get(const std::string &algorithm)
        {
            OBTAIN_LOCK(_lock);

            auto r = this->find(algorithm);

            if (r != this->end())
                return r->second;

            HashPluginRef plugin = this->load(algorithm);

            mlog(DEBUG, "Initialized hashing algorithm %s at %p", algorithm.c_str(), plugin.get());
            this->insert({ algorithm, plugin });

            return plugin;
        }

        HashPluginRef load(const std::string &algorithm)
        {
            std::string filename = "hash_" + algorithm + ".so";

            mlog(INFO, "Loading plugin %s", filename.c_str());

            void *lib = dlopen(filename.c_str(), RTLD_NOW);

            if (!lib)
                throw std::runtime_error("Failed to load module '" + filename + "' as plugin: " + dlerror());

            HASHPLUGIN_INITIALIZER initializer = (HASHPLUGIN_INITIALIZER) dlsym(lib, HASHPLUGIN_INITIALIZER_NAME);

            if (!initializer)
                throw std::runtime_error("Module '" + filename + "' does not export " HASHPLUGIN_INITIALIZER_NAME);

            return std::shared_ptr<const HashPlugin>(initializer());
        }

    private:
        Lock _lock;
};

HashPluginManager HashPluginManager::Manager;


HashPluginRef get_hashplugin(const std::string &algorithm)
{
    return HashPluginManager::Manager.get(algorithm);
}
