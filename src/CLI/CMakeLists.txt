cmake_minimum_required (VERSION 3.16)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(GinnungagapCLI)

add_executable(${PROJECT_NAME}
    GinnungagapCLI/main.cpp
    GinnungagapCLI/CSVUnpack/Unpack.cpp
    GinnungagapCLI/CSVUnpack/Pack.cpp
    GinnungagapCLI/CSVUnpack/MXEInfo.cpp
)

# Done
# Allow CMake to build libraries
cmake_policy(SET CMP0079 NEW)

target_link_libraries(${PROJECT_NAME}
  GinnungagapCore
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_dependencies(${PROJECT_NAME} copy_assets)
target_include_directories(${PROJECT_NAME} PUBLIC .)

