
add_definitions(-DMODULE=network)

add_library(socket STATIC ConnectionManager.cpp IncomingConnection.cpp Listener.cpp ListeningConnectionManager.cpp ReconnectingClient.cpp Socket.cpp SocketBase.cpp AsynchronousSocket.cpp)
set_property(TARGET socket PROPERTY POSITION_INDEPENDENT_CODE ON)