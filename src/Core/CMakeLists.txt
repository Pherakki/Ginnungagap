cmake_minimum_required (VERSION 3.16)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/../assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
project(GinnungagapCore)

add_library(${PROJECT_NAME}
    GinnungagapCore/structs/Valkyria/serialization/Structs/HeaderFlags.cpp
    GinnungagapCore/structs/Valkyria/Containers/Database/MXEN/MXEC/ParametersTable/ParamDefinitions/Definition.cpp
    GinnungagapCore/structs/Valkyria/Containers/Database/MXEN/MXEC/ParametersTable/ParamDefinitions/Load.cpp
    GinnungagapCore/structs/JSON/load.cpp
    GinnungagapCore/utils/Paths.cpp
    GinnungagapCore/utils/StringUtils.cpp
)

# Done
# Allow CMake to build libraries
cmake_policy(SET CMP0079 NEW)

target_link_libraries(${PROJECT_NAME}
  exbip
  nlohmann_json
)

if(UNIX)
 find_package(ICU COMPONENTS uc REQUIRED)
 
  target_link_libraries(${PROJECT_NAME}
  ICU::uc
  )
endif(UNIX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_dependencies(${PROJECT_NAME} copy_assets)
target_include_directories(${PROJECT_NAME} PUBLIC .)
