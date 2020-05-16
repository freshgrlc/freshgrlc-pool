#ifndef BITCOINP2P_P2PMESSAGE_H
#define BITCOINP2P_P2PMESSAGE_H

#include <memory>

#include <util/ByteString.h>


class P2PMessage;
typedef std::unique_ptr<P2PMessage> P2PMessageRef;


class P2PMessage
{
    public:
        enum Type
        {
            INVALID,

            VERSION,
            VERACK,
            PING,
            PONG,
            GETADDR,
            ADDR
        };

        P2PMessage(const std::string &cmd, const ByteString &payload = ByteString());
        P2PMessage(std::string &&cmd, ByteString &&payload);

        inline Type type(void) const                        { return _type; }
        inline const std::string &cmd(void) const           { return _type ? getCmdName(_type) : _cmd; }
        inline const ByteString &payload(void) const        { return _payload; }

        template <class T>
        inline const T &as(void) const
        {
            if (this->type() != T::Type)
                throw std::runtime_error("Invalid P2P message conversion from " + std::to_string(this->type()) + " to " + std::to_string(T::Type));

            return *((T *) this);
        }

        static P2PMessageRef parse(P2PMessage &&m);

    protected:
        inline P2PMessage(Type type) : _type(type), _cmd(), _payload() {}
        inline P2PMessage(P2PMessage &&src) : _type(src._type), _cmd(std::move(src._cmd)), _payload(std::move(src._payload)) {}

        Type _type;
        std::string _cmd;
        ByteString _payload;

    private:
        static const std::string &getCmdName(Type type);
};

#endif
