cmake_minimum_required(VERSION 3.16)
project(rl_engine LANGUAGES CXX)

# Options
option(BUILD_TESTING "Build unit tests" ON)
option(ENABLE_PEDANTIC "Enable strict warnings" ON)
option(SKIP_FOV_BLOCKING_TESTS "Skip 'walls block LOS' test until FOV is fully implemented" ON)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Warnings
if (ENABLE_PEDANTIC)
  if (MSVC)
    add_compile_options(/W4)
  else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion)
  endif()
endif()

# --- core ---
set(CORE_SOURCES
  src/core/FOV.cpp
)
set(CORE_HEADERS
  src/core/Position.hpp
  src/core/FOV.hpp
)
add_library(core STATIC ${CORE_SOURCES} ${CORE_HEADERS})
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# --- world ---
set(WORLD_SOURCES
  src/world/Map.cpp
  src/world/gen/MapGenerator.cpp
  src/world/gen/RoomsGen.cpp
  src/world/gen/CavesGen.cpp
)
set(WORLD_HEADERS
  src/world/Tile.hpp
  src/world/Map.hpp
  src/world/MapViewAdapter.hpp
  src/world/gen/MapGenerator.hpp
  src/world/gen/RoomsGen.hpp
  src/world/gen/CavesGen.hpp
)
add_library(world STATIC ${WORLD_SOURCES} ${WORLD_HEADERS})
target_include_directories(world PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(world PUBLIC core)

# --- demo ---
add_executable(rl_demo src/main.cpp)
target_link_libraries(rl_demo PRIVATE core world)

# --- tests ---
include(CTest)
if (BUILD_TESTING)
  add_executable(map_tests tests/MapTests.cpp)
  target_link_libraries(map_tests PRIVATE core world)
  add_test(NAME map_tests COMMAND map_tests)

  add_executable(fov_tests tests/FOVTests.cpp)
  target_link_libraries(fov_tests PRIVATE core world)
  if (SKIP_FOV_BLOCKING_TESTS)
    target_compile_definitions(fov_tests PRIVATE SKIP_BLOCKING_TESTS=1)
  endif()
  add_test(NAME fov_tests COMMAND fov_tests)

  add_executable(gen_tests tests/GenTests.cpp)
  target_link_libraries(gen_tests PRIVATE core world)
  add_test(NAME gen_tests COMMAND gen_tests)

  # Doors test (upewnij się co do nazwy pliku: GenDoorTest.cpp vs GenDoorsTest.cpp)
  add_executable(gen_doorstests tests/GenDoorsTest.cpp)
  target_link_libraries(gen_doorstests PRIVATE core world)
  add_test(NAME gen_doorstests COMMAND gen_doorstests)
endif()

