add_definitions(-DMODULE=mbustest)

add_executable(mbus-test main.cpp DummyClient.cpp TestMessage.cpp)
target_link_libraries(mbus-test util socket messagebus pthread)

