cmake_minimum_required(VERSION 3.16)
project(tile_system_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find nlohmann_json
find_package(nlohmann_json 3.11.0 QUIET)

if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json not found via find_package, trying FetchContent...")
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# World library (minimal for testing)
add_library(world_minimal STATIC
    ../src/world/TileRegistry.cpp
)
target_include_directories(world_minimal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(world_minimal PUBLIC nlohmann_json::nlohmann_json)

# Test executable
add_executable(tile_system_tests
    ../tests/TileSystemTests.cpp
)
target_include_directories(tile_system_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(tile_system_tests PRIVATE world_minimal)

enable_testing()
add_test(NAME tile_system_tests COMMAND tile_system_tests)
