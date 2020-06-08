
add_definitions(-DMODULE=bitcoinp2p)

add_library(bitcoinp2p STATIC P2PSocket.cpp P2PConnection.cpp P2PMessage.cpp
                              messages/PingPong.cpp messages/Version.cpp)

set_property(TARGET bitcoinp2p PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(bitcoinp2p PRIVATE socket)
