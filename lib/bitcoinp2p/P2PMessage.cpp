#include "P2PMessage.h"

#include "messages/Version.h"
#include "messages/Ping.h"


static const std::string CommandNames[] = {
    "",

    "version",
    "verack",
    "ping",
    "pong",
    "getaddr",
    "addr"
};

static const int CommandNames_len = sizeof(CommandNames)/sizeof(CommandNames[0]);

static P2PMessage::Type identify_cmd(const std::string &cmd)
{
    for (int i = 0; i < CommandNames_len; i++)
    {
        auto &entry = CommandNames[i];

        if (cmd == entry)
            return (P2PMessage::Type) i;
    }

    return P2PMessage::INVALID;
}


P2PMessage::P2PMessage(const std::string &cmd, const ByteString &payload) : _type(identify_cmd(cmd)), _cmd(cmd), _payload(payload)
{
}

P2PMessage::P2PMessage(std::string &&cmd, ByteString &&payload) : _type(identify_cmd(cmd)), _cmd(std::move(cmd)), _payload(std::move(payload))
{
}

P2PMessageRef P2PMessage::parse(P2PMessage &&m)
{
    using namespace bitcoinp2p::messages;

    switch (m.type())
    {
        case Version::Type:
            return std::make_unique<Version>(std::move(m));

        case Ping::Type:
            return std::make_unique<Ping>(std::move(m));

        default:
            class UnsupportedP2PMessage : public P2PMessage
            {
                public:
                    UnsupportedP2PMessage(P2PMessage &&src) : P2PMessage(std::move(src)) {}
            };

            return std::make_unique<UnsupportedP2PMessage>(std::move(m));
    }
}

const std::string &P2PMessage::getCmdName(P2PMessage::Type type)
{
    if (type < CommandNames_len)
        return CommandNames[type];

    return CommandNames[INVALID];
}
