
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenSSL REQUIRED)

add_definitions(-DMODULE=bitcoinrpc)

add_library(bitcoinrpc STATIC RPCConnection.cpp)

target_link_libraries(bitcoinrpc PRIVATE socket nlohmann_json::nlohmann_json OpenSSL::SSL)
