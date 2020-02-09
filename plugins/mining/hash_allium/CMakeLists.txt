
add_library(hash_allium MODULE AlliumHashPlugin.cpp)
set_target_properties(hash_allium PROPERTIES PREFIX "")

target_link_libraries(hash_allium ${bitcoinconsensus_LIBRARY})
