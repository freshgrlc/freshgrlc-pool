add_definitions(-DMODULE=crawler)

add_executable(crawler main.cpp)
target_link_libraries(crawler util socket bitcoinp2p mining pthread)

