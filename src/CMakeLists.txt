cmake_minimum_required (VERSION 3.16)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(GinnungagapSrc)

add_subdirectory(Core)
add_subdirectory(CLI)

