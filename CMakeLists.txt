cmake_minimum_required (VERSION 3.16)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(GinnungagapRoot)

add_subdirectory(external/exbip)
add_subdirectory(external/json)
add_subdirectory(src)

