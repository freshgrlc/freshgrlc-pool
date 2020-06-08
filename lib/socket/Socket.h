#ifndef SOCKET_SOCKET_H
#define SOCKET_SOCKET_H

#include "SocketBase.h"
#include "Packet.h"

#include <util/ByteString.h>

#include <memory>


class Socket : public SocketBase
{
    public:
        class StreamParser
        {
            public:
                inline StreamParser() : parent(nullptr) {}
                inline virtual ~StreamParser(void) {}

                void processPacket(const Packet &packet);

            private:
                friend class Socket;

                Socket *parent;
                ByteString buffer;

                virtual size_t getReceivedMessageSize(const ConstByteStringRef &buffer) = 0;
        };

        typedef std::unique_ptr<StreamParser> StreamParserRef;

        using SocketBase::SocketBase;
        inline Socket(SocketBase &&socket) : SocketBase(std::move(socket)) {}

        void setStreamParser(StreamParserRef &&streamParser);

        template <typename T, typename = std::enable_if<std::is_base_of<StreamParser, T>::value>>
        inline void setStreamParser(std::unique_ptr<T> &&streamParser)
        {
            this->setStreamParser((StreamParserRef &&) std::move(streamParser));
        }

        template <typename T, typename = std::enable_if<std::is_base_of<StreamParser, T>::value>>
        inline void setStreamParser(T &&streamParser)
        {
            this->setStreamParser(std::make_unique<T>(std::move(streamParser)));
        }

        void send(const Packet &packet);

    protected:
        void receive(void);

        static int openConnectionTo(const std::string &host, int port);

    private:
        StreamParserRef streamParser;

        virtual void onReceive(const Packet &packet) = 0;
};

#endif
