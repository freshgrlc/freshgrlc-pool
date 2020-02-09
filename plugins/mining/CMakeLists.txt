find_library(bitcoinconsensus_LIBRARY
    NAMES bitcoinconsensus
    PATHS ${bitcoinconsensus_LIBRARY_DIRS}
)

if(NOT bitcoinconsensus_LIBRARY)
    message(FATAL_ERROR "bitcoinconsensus library not found")
endif()

add_subdirectory(hash_allium)
add_subdirectory(hash_sha256d)
