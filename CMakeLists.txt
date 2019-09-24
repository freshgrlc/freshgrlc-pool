cmake_minimum_required(VERSION 2.6)
project(freshgrlc-pool)

set(CMAKE_CXX_STANDARD 14)

add_compile_options("-Wall" "$<$<CONFIG:RELEASE>:-O2>" "$<$<CONFIG:DEBUG>:-O0;-g3;-ggdb>")
include_directories(lib lib/util)

add_subdirectory(lib/util)
add_subdirectory(lib/socket)
add_subdirectory(lib/stratum)

add_executable(test test.cpp)
target_link_libraries(test util socket stratum pthread)
