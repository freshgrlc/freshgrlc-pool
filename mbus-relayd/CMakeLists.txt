add_definitions(-DMODULE=relayd)

add_executable(mbus-relayd main.cpp BusConnection.cpp MessageBusServer.cpp)
target_link_libraries(mbus-relayd util socket messagebus pthread)

