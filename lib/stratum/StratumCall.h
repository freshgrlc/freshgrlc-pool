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
        const int messageId;
        const std::string &method;

        virtual void process(StratumClientConnection &connection) = 0;

        static std::unique_ptr<StratumCall> parseMessage(const json &encoded);

    protected:
        StratumCall(int messageId, const std::string &method);
};

#ifdef INCLUDE_NLOHMANN_JSON_HPP_
namespace stratum { namespace messages {

template <class T>
inline T getParam(const json &params, size_t idx, const T &defaultValue)
{
    if (params.size() > idx)
        return params[idx].get<T>();

    return defaultValue;
}

}}
#endif

#endif
