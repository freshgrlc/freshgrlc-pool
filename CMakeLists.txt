cmake_minimum_required(VERSION 2.6)
project(freshgrlc-pool)

set(CMAKE_CXX_STANDARD 14)

add_compile_options("-Wall" "$<$<CONFIG:RELEASE>:-O2>" "$<$<CONFIG:DEBUG>:-O0;-g3;-ggdb>")
include_directories(lib lib/util)

if(NOT PLUGINDIR)
    include(GNUInstallDirs)
    set(PLUGINDIR "${CMAKE_INSTALL_FULL_LIBDIR}/${CMAKE_PROJECT_NAME}/plugins")
endif()

add_subdirectory(lib/util)
add_subdirectory(lib/socket)
add_subdirectory(lib/stratum)
add_subdirectory(lib/bitcoinrpc)
add_subdirectory(lib/mining)
add_subdirectory(lib/bitcoinp2p)

add_subdirectory(plugins)

add_subdirectory(pool)

add_subdirectory(utils/crawler)
