#ifndef BITCOINRPC_RPCSOCKET_H
#define BITCOINRPC_RPCSOCKET_H

#include <memory>

#include <socket/Socket.h>

class RPCConnection;

class RPCSocket;
typedef std::unique_ptr<RPCSocket> RPCSocketRef;

class RPCSocket : public Socket
{
    public:
        inline RPCSocket(const std::string &host, int port) : Socket(openConnectionTo(host, port))
        {
        }

        inline static RPCSocketRef connectToFor(const std::string &host, int port)
        {
            return std::make_unique<RPCSocket>(host, port);
        }

        inline std::string sendAndReceive(const std::string &data)
        {
            this->response = "";

            this->send(Packet(data));

            while (this->connected())
                this->receive();

            return this->response;
        }

    private:
        std::string response;

        void onReceive(const Packet &packet) override
        {
            this->response += packet.asString();
        }
};

#endif
