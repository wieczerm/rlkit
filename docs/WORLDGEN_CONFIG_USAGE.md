# Configuration System - Usage Guide

## Refactoring Goal
Eliminate "magic numbers" from generator code and centralize configuration in one place.

## Structure

```
src/
├── config/
│   └── DungeonConfig.hpp    # All configuration parameters
└── world/gen/
    ├── RoomsGen.cpp         # Uses config::LevelConfig
    └── CavesGen.cpp         # Uses config::LevelConfig
```

## Basic Usage

### 1. Using Predefined Configurations

```cpp
#include "src/config/DungeonConfig.hpp"
#include "src/world/gen/MapGenerator.hpp"

// Simple standard dungeon
auto cfg = config::standardDungeon();
MapGenerator gen;
Map m(80, 50, Tile::Wall);
gen.generateRoomsModule(m, cfg, rng);

// Small dungeon
auto tinyCfg = config::tinyDungeon();
gen.generateRoomsModule(m, tinyCfg, rng);

// Large caves
auto caveCfg = config::denseCaves();
gen.generateCavesModule(m, caveCfg, rng);
```

### 2. Custom Configuration

```cpp
#include "src/config/DungeonConfig.hpp"

config::LevelConfig myLevel;

// Customize room parameters
myLevel.rooms.max_rooms = 20;
myLevel.rooms.room_min = 4;
myLevel.rooms.room_max = 12;

// Customize cave parameters
myLevel.caves.fill_percent = 48;
myLevel.caves.steps = 7;

// Customize generation algorithm
myLevel.room_placement.attempts_multiplier = 120;
myLevel.room_placement.room_padding = 2;  // more spacing between rooms

// Use it
gen.generateRoomsModule(m, myLevel, rng);
```

### 3. Modifying Predefined Configurations

```cpp
// Start with a preset
auto cfg = config::largeDungeon();

// Modify selected parameters
cfg.rooms.add_doors = false;  // disable doors
cfg.room_placement.room_padding = 3;  // more spacing

gen.generateRoomsModule(m, cfg, rng);
```

## Available Presets

| Preset               | Description                                   |
|----------------------|-----------------------------------------------|
| `standardDungeon()`  | Balanced dungeon (default values)             |
| `tinyDungeon()`      | Small, cramped dungeon with tight corridors   |
| `largeDungeon()`     | Large, sprawling dungeon with many rooms      |
| `denseCaves()`       | Dense caves with large open spaces            |
| `tightCaves()`       | Tight, maze-like caves                        |
| `mixedLevel()`       | Mixed level: rooms with cave-like connections |

## Configuration Parameters

### RoomsOptions (from world/gen/RoomsGen.hpp)
- `max_rooms`: maximum number of rooms
- `room_min`: minimum room size
- `room_max`: maximum room size
- `add_doors`: whether to add doors

### CavesOptions (from world/gen/CavesGen.hpp)
- `fill_percent`: percentage of walls at start (0-100)
- `steps`: number of CA iterations
- `birth`: birth threshold for CA
- `survive`: survival threshold for CA
- `add_doors`: whether to add doors

### RoomPlacementConfig (new)
- `attempts_multiplier`: base number of room placement attempts
- `attempts_per_room`: additional attempts per max_rooms
- `room_padding`: minimum spacing between rooms (in tiles)
- `edge_margin`: margin from map edges
- `min_room_dimension`: absolute minimum room size

### CaveGenerationConfig (new)
- `random_percent_min`: minimum for random percentage (0-100)
- `random_percent_max`: maximum for random percentage (0-100)

## Backward Compatibility

Old API is still available for existing code:

```cpp
// Old API - still works
RoomsOptions oldOpt;
oldOpt.max_rooms = 30;
gen.generateRoomsModule(m, oldOpt, rng);

// New API - recommended
config::LevelConfig newCfg = config::standardDungeon();
gen.generateRoomsModule(m, newCfg, rng);
```

## Creating Custom Presets

```cpp
namespace config {

// Add to DungeonConfig.hpp
inline LevelConfig myCustomLevel() {
  LevelConfig cfg;
  cfg.rooms.max_rooms = 15;
  cfg.rooms.room_min = 7;
  cfg.rooms.room_max = 14;
  cfg.caves.fill_percent = 55;
  cfg.caves.steps = 4;
  cfg.room_placement.attempts_multiplier = 80;
  return cfg;
}

}
```

## Benefits of Refactoring

1. ✅ **No magic numbers** - all values are named and documented
2. ✅ **Easy experimentation** - change parameters without searching through code
3. ✅ **Reusability** - presets can be easily copied and modified
4. ✅ **Type-safe** - compile-time checking of all values
5. ✅ **Central configuration** - everything in one place
6. ✅ **Backward compatible** - old code still works
7. ✅ **Educational** - clear parameter names help understand algorithms

## Next Steps

In the future, you can extend the system with:
- Loading from JSON (when you need external config)
- Validation functions for parameters
- More presets for different level types
- Parameters for other aspects (spawning, treasure, difficulty)
