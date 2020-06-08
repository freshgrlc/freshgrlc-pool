
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(.)

add_definitions(-DMODULE=stratum)

add_library(stratum STATIC
            BlockGenerationInformation.cpp JobManager.cpp NetworkState.cpp
            StratumClientConnection.cpp StratumJob.cpp StratumServer.cpp
            StratumCall.cpp
            calls/MiningAuthorize.cpp calls/MiningExtraNonceSubscribe.cpp calls/MiningSubmit.cpp calls/MiningSubscribe.cpp)

set_property(TARGET stratum PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(stratum PRIVATE OpenSSL::SSL)
