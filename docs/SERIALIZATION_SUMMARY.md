# Serialization System Implementation Summary

## Files Created

### Core Serialization (Basic Types)
1. **src/core/Serialization.hpp** - Declarations for basic type serialization
2. **src/core/Serialization.cpp** - Implementations (Position, Tile, Feature, Entity)

### Game State Serialization
3. **src/serialization/GameSerialization.hpp** - LevelState, SaveData structures and save/load functions
4. **src/serialization/GameSerialization.cpp** - Implementations (Map, managers, game state)

### Testing
5. **tests/SerializationTests.cpp** - Comprehensive test suite (13 tests)

## Architecture Overview

### Serialization Layers

**Layer 1: Basic Types (Serialization.hpp/cpp)**
- Position
- Tile enum
- Feature variant (Door, Stairs)
- Entity (with AI type string)
- AI factory function

**Layer 2: Managers (GameSerialization.cpp)**
- Map
- FeatureManager
- EntityManager

**Layer 3: Game State (GameSerialization.hpp/cpp)**
- LevelState (complete level snapshot)
- SaveData (all visited levels + metadata)
- File I/O functions

## Data Structures

### LevelState
```cpp
struct LevelState {
    Map map;                        // Terrain tiles
    FeatureManager features;        // Doors, stairs
    EntityManager entities;         // Creatures (AI recreated on load)
    std::vector<bool> discovered;   // Discovered tiles for map memory
    int depth;                      // Level depth
};
```

### SaveData
```cpp
struct SaveData {
    std::vector<LevelState> visited_levels;  // Stack of visited levels
    int current_level_index;                 // Active level
    int turn_counter;                        // Total game turns
};
```

## JSON Format

### Example Save File Structure
```json
{
  "turn_counter": 1523,
  "current_level_index": 1,
  "visited_levels": [
    {
      "depth": 1,
      "map": {
        "width": 80,
        "height": 24,
        "tiles": [0, 0, 1, 0, ...]  // Enum values
      },
      "discovered": [true, true, false, ...],
      "features": [
        {
          "position": {"x": 10, "y": 5},
          "feature": {
            "type": "Door",
            "material": 0,
            "state": 1
          }
        }
      ],
      "entities": [
        {
          "name": "goblin",
          "position": {"x": 15, "y": 8},
          "glyph": "g",
          "properties": {"hp": 20, "str": 8, "speed": 100},
          "ai_type": "SimpleAI"
        }
      ]
    }
  ]
}
```

## Key Design Features

### AI Serialization
- Entities store AI type as string ("SimpleAI")
- `createAIFromType()` factory function recreates AI on load
- Simple string matching (extend when adding new AI types)

### Compact Encoding
- Tiles: stored as enum integers (0, 1, 2, ...) not strings
- Feature enums: stored as integers
- Discovered tiles: boolean array (~240 bytes per 80x24 level)

### Discovered Tiles (Map Memory)
- Parallel to map tiles (same size, indexing)
- Serialized as boolean array
- Restored on load for continuity
- Used for grayed-out rendering when out of FOV

### Error Handling
- File I/O errors throw `std::runtime_error`
- JSON parse errors throw with descriptive messages
- Caller (Game class) handles with try/catch

## Integration with Game Class

### Save Game Flow
```cpp
class Game {
    void saveGame() {
        // Build SaveData
        SaveData data;
        data.turn_counter = turnCounter_;
        data.current_level_index = currentDepth_;
        
        // Add current level
        data.visited_levels.push_back(LevelState(
            map_,
            featureManager_,
            entityManager_,
            discoveredTiles_,
            depth_
        ));
        
        // Add visited levels from stack
        for (auto& level : levelStack_) {
            data.visited_levels.push_back(level);
        }
        
        // Save to file
        try {
            serialization::saveGame(data, "savegame.json");
            addMessage("Game saved!");
        } catch (const std::exception& e) {
            addMessage("Save failed: " + std::string(e.what()));
        }
    }
};
```

### Load Game Flow
```cpp
void Game::loadGame() {
    try {
        SaveData data = serialization::loadGame("savegame.json");
        
        turnCounter_ = data.turn_counter;
        
        // Restore current level
        LevelState& current = data.visited_levels[data.current_level_index];
        map_ = std::move(current.map);
        featureManager_ = std::move(current.features);
        entityManager_ = std::move(current.entities);
        discoveredTiles_ = std::move(current.discovered);
        depth_ = current.depth;
        
        // Rebuild turn manager from entities
        rebuildTurnManager();
        
        // Recompute FOV for current player position
        computeFOV();
        
        addMessage("Game loaded!");
        
    } catch (const std::exception& e) {
        addMessage("Load failed: " + std::string(e.what()));
    }
}
```

### Level Transition with Stack
```cpp
void Game::descendStairs() {
    // Process all events first
    processEventQueue();
    
    // Save current level to stack
    LevelState current_level(
        std::move(map_),
        std::move(featureManager_),
        std::move(entityManager_),
        std::move(discoveredTiles_),
        depth_
    );
    levelStack_.push(std::move(current_level));
    
    // Generate new level
    generateLevel(++depth_);
    
    // Auto-save after transition
    saveGame();
}

void Game::ascendStairs() {
    // Process all events first
    processEventQueue();
    
    if (levelStack_.empty()) {
        addMessage("Can't go up from here");
        return;
    }
    
    // Restore previous level
    LevelState previous = levelStack_.pop();
    map_ = std::move(previous.map);
    featureManager_ = std::move(previous.features);
    entityManager_ = std::move(previous.entities);
    discoveredTiles_ = std::move(previous.discovered);
    depth_ = previous.depth;
    
    // Rebuild turn manager
    rebuildTurnManager();
    
    // Recompute FOV
    computeFOV();
    
    // Auto-save
    saveGame();
}
```

## Map Memory System (Replaces Minimap)

### Rendering with Discovered Tiles
```cpp
char getGlyphAt(Position pos) {
    // Not discovered = show nothing
    if (!discoveredTiles_[pos.y * map_.width() + pos.x]) {
        return ' ';
    }
    
    // In FOV = show everything
    if (fov_.isVisible(pos)) {
        // Priority: Entity > Feature > Tile
        if (Entity* e = entityManager_.getEntityAt(pos)) {
            return e->getGlyph();
        }
        if (Feature* f = featureManager_.getFeature(pos)) {
            return getGlyph(*f);
        }
        return getTileGlyph(map_.at(pos));
    }
    
    // Out of FOV but discovered = show terrain only (grayed)
    if (Feature* f = featureManager_.getFeature(pos)) {
        return getGlyph(*f);  // Show doors, stairs (grayed)
    }
    return getTileGlyph(map_.at(pos));  // Show terrain (grayed)
}
```

## CMakeLists.txt Changes

Add to CORE_SOURCES:
```cmake
src/core/Serialization.cpp
```

Add to CORE_HEADERS:
```cmake
src/core/Serialization.hpp
```

Add new serialization library:
```cmake
add_library(serialization STATIC
    src/serialization/GameSerialization.cpp
)
target_include_directories(serialization PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(serialization PUBLIC core world entities nlohmann_json::nlohmann_json)
```

Update game executable:
```cmake
target_link_libraries(rl_demo PRIVATE core world entities serialization ...)
```

Add test:
```cmake
add_executable(serialization_tests tests/SerializationTests.cpp)
target_link_libraries(serialization_tests PRIVATE core world entities ai serialization)
add_test(NAME serialization_tests COMMAND serialization_tests)
```

## Test Coverage

✅ Position serialization round-trip
✅ Tile enum serialization
✅ Door feature serialization (all states)
✅ Stairs feature serialization (both directions)
✅ Entity serialization (with and without AI)
✅ AI recreation from type string
✅ Map serialization (terrain grid)
✅ FeatureManager serialization (all features)
✅ EntityManager serialization (all entities)
✅ LevelState serialization (complete level snapshot)
✅ SaveData serialization (multiple levels)
✅ File save/load round-trip
✅ Error handling for missing files

## Testing Instructions

```bash
# Compile (adjust paths as needed)
g++ -std=c++20 -I. -Isrc \
    src/core/Serialization.cpp \
    src/serialization/GameSerialization.cpp \
    src/world/FeatureManager.cpp \
    src/world/TileRegistry.cpp \
    src/world/TileEnum.cpp \
    src/entities/Entity.cpp \
    src/entities/EntityManager.cpp \
    src/world/Map.cpp \
    src/ai/SimpleAI.cpp \
    tests/SerializationTests.cpp \
    -o serialization_tests

# Run
./serialization_tests

# Expected output:
# === Serialization System Tests ===
# Testing Position serialization...
#   ✓ Position round-trip successful
# ...
# ✓ All tests passed!
```

## Future Extensions

### Additional Metadata
```cpp
struct SaveData {
    // ... existing fields
    std::string player_name;
    std::time_t save_time;
    std::string version;  // Game version for compatibility
};
```

### Compressed Saves
```cpp
// Use zlib or similar for compression
void saveGame(const SaveData& data, const std::string& filename) {
    json j = data;
    std::string json_str = j.dump();
    std::string compressed = compress(json_str);
    writeToFile(filename, compressed);
}
```

### Auto-save System
```cpp
class Game {
    void autoSave() {
        if (turnCounter_ % 100 == 0) {  // Every 100 turns
            saveGame("autosave.json");
        }
    }
};
```

## Integration Checklist

- [ ] Add `std::vector<bool> discoveredTiles_` to Game class
- [ ] Add `std::vector<LevelState> levelStack_` to Game class
- [ ] Implement `Game::saveGame()` method
- [ ] Implement `Game::loadGame()` method
- [ ] Update `Game::descendStairs()` to push level to stack
- [ ] Implement `Game::ascendStairs()` to pop from stack
- [ ] Update renderer to use discovered tiles for map memory
- [ ] Add save/load commands to input system
- [ ] Add auto-save on level transitions
- [ ] Test full save/load cycle in actual game

## Notes

- Serialization uses nlohmann::json's automatic detection of to_json/from_json
- TurnManager is NOT serialized - rebuilt from entities on load
- FOV state is NOT serialized - recomputed on load
- Message log is NOT serialized - not critical for save state
- Discovered tiles ARE serialized - critical for map memory continuity
- Save files are human-readable JSON (pretty-printed with 2-space indent)
- File format is extensible - easy to add new fields

## Next Steps

After integrating Serialization System:
1. Test save/load independently
2. Implement LevelStack in Game class
3. Update level transition logic (descend/ascend)
4. Update renderer for map memory (discovered tiles)
5. Add save/load UI commands
6. Test full game cycle with multiple levels and backtracking
