
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(.)

add_definitions(-DMODULE=stratum)

add_library(stratum STATIC
            NetworkState.cpp StratumCall.cpp StratumClientConnection.cpp StratumJob.cpp StratumServer.cpp
            calls/MiningAuthorize.cpp calls/MiningExtraNonceSubscribe.cpp calls/MiningSubscribe.cpp)

target_link_libraries(stratum PRIVATE nlohmann_json::nlohmann_json OpenSSL::SSL)
