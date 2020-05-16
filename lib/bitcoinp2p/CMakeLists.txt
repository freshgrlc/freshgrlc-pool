
add_definitions(-DMODULE=bitcoinp2p)

add_library(bitcoinp2p STATIC P2PSocket.cpp P2PConnection.cpp P2PMessage.cpp
                              messages/PingPong.cpp messages/Version.cpp)

target_link_libraries(bitcoinp2p PRIVATE socket)
