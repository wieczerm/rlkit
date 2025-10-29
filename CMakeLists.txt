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
  src/core/Pathfinding.cpp
  src/core/InputHandler.cpp
)
set(CORE_HEADERS
  src/core/Position.hpp
  src/core/FOV.hpp
  src/core/Pathfinding.hpp
  src/core/InputHandler.hpp
)
add_library(core STATIC ${CORE_SOURCES} ${CORE_HEADERS})
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# In systems section (new)
set(SYSTEMS_SOURCES
  src/systems/CombatSystem.cpp
)
set(SYSTEMS_HEADERS
  src/systems/CombatSystem.hpp
)
add_library(systems STATIC ${SYSTEMS_SOURCES} ${SYSTEMS_HEADERS})
target_include_directories(systems PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(systems PUBLIC entities)

# In actions section (new)
set(ACTIONS_SOURCES
  src/actions/MoveAction.cpp
  src/actions/OpenAction.cpp
)
set(ACTIONS_HEADERS
  src/actions/ActionResult.hpp
  src/actions/MoveAction.hpp
  src/actions/OpenAction.hpp
)
add_library(actions STATIC ${ACTIONS_SOURCES} ${ACTIONS_HEADERS})
target_include_directories(actions PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(actions PUBLIC core world entities systems)


# --- entities ---
set(ENTITIES_SOURCES
  src/entities/Entity.cpp
  src/entities/EntityManager.cpp
  src/entities/TurnManager.cpp
)
set(ENTITIES_HEADERS  
  src/entities/Entity.hpp
  src/entities/EntityManager.hpp
  src/entities/TurnManager.hpp
)
add_library(entities STATIC ${ENTITIES_SOURCES} ${ENTITIES_HEADERS})
target_include_directories(entities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(entities PUBLIC core)

# --- ai ---
set(AI_SOURCES
  src/ai/SimpleAI.cpp
)
set(AI_HEADERS
  src/ai/AIBehavior.hpp
  src/ai/SimpleAI.hpp
)
add_library(ai STATIC ${AI_SOURCES} ${AI_HEADERS})
target_include_directories(ai PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ai PUBLIC core world entities actions)


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

find_package(ftxui REQUIRED)

# --- FTXUI renderer ---
set(FTXUIRenderer_SOURCES
  src/renderers/FTXUIRenderer.cpp
)
set(FTXUIRenderer_HEADERS
  src/renderers/FTXUIRenderer.hpp
)

add_library(ftxui_renderer STATIC ${FTXUIRenderer_SOURCES} ${FTXUIRenderer_HEADERS})
target_include_directories(ftxui_renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ftxui_renderer PUBLIC 
  core 
  world 
  entities
  ftxui::screen 
  ftxui::dom 
  ftxui::component
)

# --- demo ---
add_executable(rl_demo src/main.cpp)
target_link_libraries(rl_demo PRIVATE core world entities ftxui_renderer systems actions ai)

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

  add_executable(pathfinding_tests tests/PathfindingTests.cpp)
  target_link_libraries(pathfinding_tests PRIVATE core world)
  add_test(NAME pathfinding_tests COMMAND pathfinding_tests)

  add_executable(entities_tests tests/EntityTests.cpp)
  target_link_libraries(entities_tests PRIVATE core world entities)
  add_test(NAME entities_tests COMMAND entities_tests)

  add_executable(entity_manager_tests tests/EntityManagerTests.cpp)
  target_link_libraries(entity_manager_tests PRIVATE core world entities)
  add_test(NAME entity_manager_tests COMMAND entity_manager_tests)

  add_executable(turn_manager_tests tests/TurnManagerTests.cpp)
  target_link_libraries(turn_manager_tests PRIVATE core world entities)
  add_test(NAME turn_manager_tests COMMAND turn_manager_tests)

endif()

