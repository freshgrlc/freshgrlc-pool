
add_library(hash_sha256d MODULE SHA256dHashPlugin.cpp)
set_target_properties(hash_sha256d PROPERTIES PREFIX "")

target_link_libraries(hash_sha256d ${bitcoinconsensus_LIBRARY})
