#pragma once
#include "../world/gen/RoomsGen.hpp"
#include "../world/gen/CavesGen.hpp"

namespace config {

// ============================================================================
// Generation Algorithm Configuration
// ============================================================================

// Configuration for room placement algorithm
struct RoomPlacementConfig {
  int attempts_multiplier = 100;  // Base attempts for room placement
  int attempts_per_room = 8;      // Additional attempts per max_rooms
  int room_padding = 1;           // Minimum gap between rooms (in tiles)
  int edge_margin = 2;            // Margin from map edges
  int min_room_dimension = 2;     // Absolute minimum room size
};

// Configuration for cellular automata cave generation
struct CaveGenerationConfig {
  int random_percent_min = 0;     // Minimum for random percentage (0-100)
  int random_percent_max = 99;    // Maximum for random percentage (0-100)
};

// ============================================================================
// Level Configuration
// ============================================================================

// Complete configuration for a single dungeon level
struct LevelConfig {
  world::RoomsOptions rooms;
  world::CavesOptions caves;
  RoomPlacementConfig room_placement;
  CaveGenerationConfig cave_generation;
  
  // Constructor with sensible defaults
  LevelConfig() {
    // RoomsOptions defaults (already defined in RoomsGen.hpp)
    rooms.max_rooms = 35;
    rooms.room_min = 5;
    rooms.room_max = 10;
    rooms.add_doors = true;
    
    // CavesOptions defaults (already defined in CavesGen.hpp)
    caves.fill_percent = 52;
    caves.steps = 5;
    caves.birth = 5;
    caves.survive = 4;
    caves.add_doors = true;
  }
};

// ============================================================================
// Preset Level Configurations
// ============================================================================

// Small, cramped dungeon with tight corridors
inline LevelConfig tinyDungeon() {
  LevelConfig cfg;
  cfg.rooms.max_rooms = 10;
  cfg.rooms.room_min = 3;
  cfg.rooms.room_max = 6;
  return cfg;
}

// Standard balanced dungeon
inline LevelConfig standardDungeon() {
  return LevelConfig(); // Use defaults
}

// Large, sprawling dungeon with many rooms
inline LevelConfig largeDungeon() {
  LevelConfig cfg;
  cfg.rooms.max_rooms = 50;
  cfg.rooms.room_min = 6;
  cfg.rooms.room_max = 15;
  cfg.room_placement.attempts_multiplier = 150;
  return cfg;
}

// Dense cave system with lots of open space
inline LevelConfig denseCaves() {
  LevelConfig cfg;
  cfg.caves.fill_percent = 45;  // Less initial walls = more open
  cfg.caves.steps = 6;
  cfg.caves.birth = 5;
  cfg.caves.survive = 3;
  return cfg;
}

// Tight, maze-like caves
inline LevelConfig tightCaves() {
  LevelConfig cfg;
  cfg.caves.fill_percent = 58;  // More initial walls = tighter
  cfg.caves.steps = 4;
  cfg.caves.birth = 6;
  cfg.caves.survive = 5;
  return cfg;
}

// Mixed level: rooms with cave-like connections
inline LevelConfig mixedLevel() {
  LevelConfig cfg;
  cfg.rooms.max_rooms = 25;
  cfg.rooms.room_min = 4;
  cfg.rooms.room_max = 8;
  cfg.caves.fill_percent = 50;
  cfg.caves.steps = 3;
  return cfg;
}

} // namespace config
