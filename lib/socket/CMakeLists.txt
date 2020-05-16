
add_definitions(-DMODULE=network)

add_library(socket STATIC ConnectionManager.cpp IncomingConnection.cpp Listener.cpp ListeningConnectionManager.cpp Socket.cpp SocketBase.cpp AsynchronousSocket.cpp)
