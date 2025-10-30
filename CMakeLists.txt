cmake_minimum_required(VERSION 3.16)
project(rl_engine LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Warnings
if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion)
endif()

# Find required packages
find_package(ftxui REQUIRED)
find_package(nlohmann_json 3.11.0 REQUIRED)

# --- core ---
set(CORE_SOURCES
  src/core/FOV.cpp
  src/core/Pathfinding.cpp
  src/core/InputHandler.cpp
  src/core/InputMapper.cpp
  src/core/InputScheme.cpp
  src/core/Event.cpp
  src/core/EventQueue.cpp
  src/core/Serialization.cpp
)
set(CORE_HEADERS
  src/core/Position.hpp
  src/core/FOV.hpp
  src/core/Pathfinding.hpp
  src/core/InputHandler.hpp
  src/core/InputMapper.hpp
  src/core/InputScheme.hpp
  src/core/InputAction.hpp
  src/core/IMapView.hpp
  src/core/Types.hpp
  src/core/Event.hpp
  src/core/EventQueue.hpp
  src/core/Serialization.hpp
)
add_library(core STATIC ${CORE_SOURCES} ${CORE_HEADERS})
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

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
target_include_directories(entities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(entities PUBLIC core)

# --- systems ---
set(SYSTEMS_SOURCES
  src/systems/CombatSystem.cpp
)
set(SYSTEMS_HEADERS
  src/systems/CombatSystem.hpp
)
add_library(systems STATIC ${SYSTEMS_SOURCES} ${SYSTEMS_HEADERS})
target_include_directories(systems PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(systems PUBLIC entities)

# --- world ---
set(WORLD_SOURCES
  src/world/TileRegistry.cpp
  src/world/TileEnum.cpp
  src/world/Map.cpp
  src/world/FeatureManager.cpp
  src/world/gen/MapGenerator.cpp
  src/world/gen/RoomsGen.cpp
  src/world/gen/CavesGen.cpp
  src/world/gen/LevelGenerator.cpp
  src/world/gen/FeaturePlacer.cpp
)
set(WORLD_HEADERS
  src/world/Tile.hpp
  src/world/TileProperties.hpp
  src/world/TileRegistry.hpp
  src/world/TileEnum.hpp
  src/world/Map.hpp
  src/world/MapViewAdapter.hpp
  src/world/Feature.hpp
  src/world/FeatureManager.hpp
  src/world/FeatureProperties.hpp
  src/world/gen/MapGenerator.hpp
  src/world/gen/RoomsGen.hpp
  src/world/gen/CavesGen.hpp
  src/world/gen/GenOptions.hpp
  src/world/gen/LevelData.hpp
  src/world/gen/LevelGenerator.hpp
  src/world/gen/FeaturePlacer.hpp
)
add_library(world STATIC ${WORLD_SOURCES} ${WORLD_HEADERS})
target_include_directories(world PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(world PUBLIC core nlohmann_json::nlohmann_json)

# --- actions ---
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
target_include_directories(actions PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(actions PUBLIC core world entities systems)

# --- ai ---
set(AI_SOURCES
  src/ai/SimpleAI.cpp
)
set(AI_HEADERS
  src/ai/AIBehavior.hpp
  src/ai/SimpleAI.hpp
)
add_library(ai STATIC ${AI_SOURCES} ${AI_HEADERS})
target_include_directories(ai PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(ai PUBLIC core world entities actions)

# --- renderers ---
set(RENDERER_SOURCES
  src/renderers/FTXUIRenderer.cpp
)
set(RENDERER_HEADERS
  src/renderers/FTXUIRenderer.hpp
)
add_library(ftxui_renderer STATIC ${RENDERER_SOURCES} ${RENDERER_HEADERS})
target_include_directories(ftxui_renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(ftxui_renderer PUBLIC 
  core 
  world 
  entities
  ftxui::screen 
  ftxui::dom 
  ftxui::component
)

# --- ui ---
set(UI_SOURCES
  src/ui/UIManager.cpp
  src/ui/GameViewPanel.cpp
  src/ui/MessageLogPanel.cpp
  src/ui/LookInfoPanel.cpp
  src/ui/MinimapPanel.cpp
  src/ui/StatusBarPanel.cpp
  src/ui/StatsBarPanel.cpp
)
set(UI_HEADERS
  src/ui/Panel.hpp
  src/ui/UIManager.hpp
  src/ui/GameViewPanel.hpp
  src/ui/MessageLogPanel.hpp
  src/ui/LookInfoPanel.hpp
  src/ui/MinimapPanel.hpp
  src/ui/StatusBarPanel.hpp
  src/ui/StatsBarPanel.hpp
)
add_library(ui STATIC ${UI_SOURCES} ${UI_HEADERS})
target_include_directories(ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(ui PUBLIC core world entities)

# --- demo executable ---
add_executable(rl_demo
  src/main.cpp
  src/Game.cpp
  src/Game.hpp
)
target_link_libraries(rl_demo PRIVATE 
  core 
  world 
  entities 
  ftxui_renderer 
  systems 
  actions 
  ai 
  ui
)
