cmake_minimum_required (VERSION 3.16)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(exbip)

add_library(${PROJECT_NAME}
    INTERFACE
)


# Done
# Allow CMake to build libraries
cmake_policy(SET CMP0079 NEW)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

target_include_directories(${PROJECT_NAME} INTERFACE .)

