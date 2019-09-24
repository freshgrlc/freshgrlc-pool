#ifndef SOCKET_CONNECTIONMANAGER_H
#define SOCKET_CONNECTIONMANAGER_H

#include <memory>
#include <vector>
#include <pthread.h>

#include "Listener.h"
#include "Packet.h"

#include <Lock.h>


class ConnectionManager
{
    public:
        class Connection : public Socket
        {
            public:
                Connection(ConnectionManager &manager, Socket &&socket);

                void send(const Packet &packet);

                inline void enableProcessing(void)  { this->receiver.run(); }
                inline bool dangling(void)          { return _dangling; }

            private:
                class ReceiverThread
                {
                    public:
                        enum State
                        {
                            FailedToStart,
                            WaitingToStart,
                            Running,
                            Stopped,
                            Terminating
                        };

                        ReceiverThread(Connection *parent);
                        virtual ~ReceiverThread(void);

                        void run(void);

                        inline State state(void)    { return _state; }

                    private:
                        Connection &parent;
                        pthread_t _thread;
                        Lock _thread_run_lock;
                        State _state;

                        void receiver(void);
                        static void *entrypoint(void *context);
                };

                ReceiverThread receiver;
                ConnectionManager &_manager;
                bool _dangling;

                void receive(void);
                void cleanup(void);

                virtual void onReceive(const Packet &packet) = 0;
        };

        ConnectionManager(Listener &listener);

        void addConnection(Socket &&socket);
        static void addConnection(Socket &&socket, void *manager);

        int listen(void);

        inline Listener &listener(void)                                         { return _listener; }
        inline const std::vector<std::unique_ptr<Connection>> &clients(void)    { return _clients; }

    private:
        Listener &_listener;
        std::vector<std::unique_ptr<Connection>> _clients;
        Lock _clients_lock;

        void remove(Connection &connection);

        virtual std::unique_ptr<Connection> makeConnection(Socket &&socket, ConnectionManager &manager) = 0;
};

#endif
