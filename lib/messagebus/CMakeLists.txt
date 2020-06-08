
add_definitions(-DMODULE=messagebus)

add_library(messagebus SHARED AbstractMessage.cpp MessageBusClient.cpp MessageFactory.cpp MessageStreamParser.cpp OpaqueMessage.cpp SetSubscriptionsMessage.cpp)
target_link_libraries(messagebus socket util)

