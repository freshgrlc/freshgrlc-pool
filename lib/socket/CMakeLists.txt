
add_definitions(-DMODULE=network)

add_library(socket STATIC ConnectionManager.cpp Listener.cpp Socket.cpp)
