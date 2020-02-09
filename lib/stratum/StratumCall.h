#ifndef STRATUM_STRATUMCALL_H
#define STRATUM_STRATUMCALL_H

#include <string>
#include <memory>

#ifndef INCLUDE_NLOHMANN_JSON_HPP_
namespace nlohmann {
    class json;
};
#endif

using json = nlohmann::json;


class StratumClientConnection;

class StratumCall
{
    public:
        inline virtual ~StratumCall(void) {}

        const int messageId;
        const std::string &method;

        virtual void process(StratumClientConnection &connection) = 0;

        static std::unique_ptr<StratumCall> parseMessage(const json &encoded);

    protected:
        StratumCall(int messageId, const std::string &method);
};

#ifdef INCLUDE_NLOHMANN_JSON_HPP_

#include <util/ByteString.h>

namespace stratum { namespace messages {

template <class T>
inline T getParam(const json &params, size_t idx, const T &defaultValue)
{
    if (params.size() > idx)
        return params[idx].get<T>();

    return defaultValue;
}

template <>
inline ByteString getParam(const json &params, size_t idx, const ByteString &defaultValue)
{
    if (params.size() > idx)
        return ByteString::fromHex(params[idx].get<std::string>());

    return defaultValue;
}

}}
#endif

#endif
