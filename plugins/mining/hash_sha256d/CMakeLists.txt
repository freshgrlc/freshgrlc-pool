
include(CheckCXXSymbolExists)

set(CMAKE_REQUIRED_FLAGS -I${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_REQUIRED_LIBRARIES ${bitcoinconsensus_LIBRARY})
check_cxx_symbol_exists(csha256_exists_symbol test_csha256_exists.h HAVE_CSHA256)

if(HAVE_CSHA256)
    add_library(hash_sha256d MODULE SHA256dHashPlugin.cpp)
    set_target_properties(hash_sha256d PROPERTIES PREFIX "")

    target_link_libraries(hash_sha256d ${bitcoinconsensus_LIBRARY})
    install(TARGETS hash_sha256d LIBRARY DESTINATION ${PLUGINDIR})
else(HAVE_CSHA256)
    message(FATAL_ERROR "Error: class CSHA256 not defined in ${bitcoinconsensus_LIBRARY}, cannot build required target hash_sha256d")
endif(HAVE_CSHA256)
