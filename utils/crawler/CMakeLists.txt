add_definitions(-DMODULE=crawler)

add_executable(crawler main.cpp ConnectionManager.cpp ConnectionHolder.cpp NodeConnection.cpp)
target_link_libraries(crawler util socket bitcoinp2p mining pthread)

