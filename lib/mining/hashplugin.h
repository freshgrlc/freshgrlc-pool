#ifndef MINING_HASHPLUGIN_H
#define MINING_HASHPLUGIN_H

#include <interfaces/plugins/HashPlugin.h>

HashPluginRef get_hashplugin(const std::string &algorithm);

#endif
