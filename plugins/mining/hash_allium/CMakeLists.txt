
include(CheckSymbolExists)

set(CMAKE_REQUIRED_FLAGS -I${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_REQUIRED_LIBRARIES ${bitcoinconsensus_LIBRARY})
check_symbol_exists(allium_hash allium_hash.h HAVE_ALLIUM_HASH)

if(HAVE_ALLIUM_HASH)
    add_library(hash_allium MODULE AlliumHashPlugin.cpp)
    set_target_properties(hash_allium PROPERTIES PREFIX "")

    target_link_libraries(hash_allium ${bitcoinconsensus_LIBRARY})
    install(TARGETS hash_allium LIBRARY DESTINATION ${PLUGINDIR})
else(HAVE_ALLIUM_HASH)
    message(WARNING "Skipping building target hash_allium: Hashing algorithm not available in ${bitcoinconsensus_LIBRARY}")
endif(HAVE_ALLIUM_HASH)
