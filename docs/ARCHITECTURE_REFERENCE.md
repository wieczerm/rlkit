# Architecture Reference - Level Generation System

## Quick Overview

```
Level Generation Flow:
Game::generateLevel() 
  → LevelGenerator::generateLevel()
    → generateTerrain() [calls RoomsGen/CavesGen]
    → placeFeatures() [calls FeaturePlacer]
      → placeDoors()
      → placeStairs()
    → findSpawnPoints()
  → Returns LevelData
  → Game takes ownership of Map and FeatureManager
```

## Component Responsibilities

### LevelGenerator (Orchestrator)
**What it does**: Coordinates entire level generation process
**Input**: Width, height, depth, config
**Output**: LevelData (map, features, spawn points)
**Does NOT**: Directly create tiles or features

### FeaturePlacer (Feature Placement)
**What it does**: Places doors and stairs on existing terrain
**Input**: Map (terrain), FeatureManager (to populate), RNG
**Output**: Modified FeatureManager with placed features
**Does NOT**: Modify terrain (Map)

### RoomsGen / CavesGen (Terrain Generators)
**What they do**: Create Floor and Wall tiles only
**Input**: Map reference, config, RNG
**Output**: Map filled with terrain
**Do NOT**: Place doors, stairs, or any features

### LevelData (Data Transfer)
**What it is**: Complete level package
**Contains**: 
- unique_ptr<Map> - terrain
- unique_ptr<FeatureManager> - features
- player_spawn position
- monster_spawns vector
- metadata (depth, room_count)
**Purpose**: Clean transfer of ownership from generator to game

## Data Flow

### Generation Phase
```
1. LevelGenerator creates empty Map
2. LevelGenerator creates empty FeatureManager
3. RoomsGen/CavesGen fills Map with Floor/Wall
4. FeaturePlacer scans Map and adds Features
5. FeaturePlacer finds valid spawn positions
6. LevelGenerator packages everything in LevelData
7. Game takes ownership via std::move
```

### Runtime Phase
```
Player Action
  → Action checks Map (terrain blocking)
  → Action checks FeatureManager (feature blocking)
  → Action modifies FeatureManager (open door)
  → Action modifies Entity position

AI Action
  → AI computes FOV/Pathfinding (uses Map)
  → AI creates MoveAction
  → MoveAction checks Map and Features
  → Entity moves if not blocked

Rendering
  → Check FOV (uses Map for LOS)
  → Render layers: Terrain → Features → Entities → Cursor
```

## Layer System

### Rendering Order (bottom to top)
1. **Terrain Layer** (Map): Floor '.', Wall '#'
2. **Feature Layer** (FeatureManager): Stairs '>' '<', Doors '+' '\''
3. **Entity Layer** (EntityManager): Player '@', Monsters 'g'
4. **UI Layer**: Cursor 'X' (in look mode)

### Blocking Priority
1. **Terrain**: Wall always blocks
2. **Features**: Closed door blocks, stairs don't
3. **Entities**: Block movement (triggers bump attack)

## Key Interfaces

### Map (Terrain)
```cpp
bool blocksMovement(Position pos);
bool blocksLineOfSight(Position pos);
Tile at(Position pos);
void set(Position pos, Tile tile);
```

### FeatureManager (Features)
```cpp
void addFeature(Position pos, Feature feature);
Feature* getFeature(Position pos);
bool hasFeature(Position pos);
bool blocksMovement(Position pos);
bool blocksLineOfSight(Position pos);
```

### Feature (Variant)
```cpp
variant<Door, Stairs> Feature;

Door: {Material, State}
Stairs: {Direction, target_depth}
```

## Feature Properties

### Doors
- **Closed**: Blocks movement and LOS
- **Open**: Does not block
- **Glyph**: '+' closed, '\'' open
- **Interaction**: Can be opened with 'o' key

### Stairs
- **Down**: Leads to deeper level
- **Up**: Not yet implemented (future)
- **Glyph**: '>' down, '<' up
- **Interaction**: Use '>' key to descend
- **Blocking**: Never blocks movement or LOS

## Configuration

### Level Generation Config
```cpp
struct LevelConfig {
  GeneratorType generator_type;
  RoomsOptions rooms;
  CavesOptions caves;
  RoomPlacementConfig room_placement;
};
```

### Stairs Placement
- Hardcoded in FeaturePlacer::placeStairs()
- First stairs: Guaranteed
- Second stairs: 15% probability
- Third stairs: 5% probability
- Min distance from walls: 2 tiles
- Must have 2+ walkable neighbors

### Door Placement
- Automatic detection at corridor-room junctions
- Horizontal: floor left+right, wall up+down
- Vertical: floor up+down, wall left+right
- Always placed as closed

### Spawn Points
- Player: Single position, 5+ tiles from stairs
- Monsters: Up to 20 positions, 5+ tiles from stairs
- Must be on floor, not blocked, no feature

## Common Patterns

### Adding New Feature Type
```cpp
// 1. Define in Feature.hpp
struct Trap {
  enum class Type { Spike, Fire, Teleport };
  Type type;
  bool revealed;
};

// 2. Update Feature variant
using Feature = std::variant<Door, Stairs, Trap>;

// 3. Add properties in FeatureProperties.hpp
inline bool blocksMovement(const Feature& f) {
  // ... existing code ...
  if constexpr (std::is_same_v<T, Trap>) {
    return false; // Traps don't block
  }
}

// 4. Add placement logic in FeaturePlacer
void FeaturePlacer::placeTraps(Map& map, FeatureManager& features) {
  // Implementation
}

// 5. Call from LevelGenerator
void LevelGenerator::placeFeatures(...) {
  placer_.placeDoors(map, features);
  placer_.placeStairs(map, features, target_depth);
  placer_.placeTraps(map, features); // NEW
}
```

### Checking Feature Type
```cpp
// Get feature
const Feature* f = featureMgr_->getFeature(pos);

// Type check
if (f && isStairs(*f)) {
  const Stairs* stairs = getStairs(*f);
  // Use stairs
}

// Property check
if (f && blocksMovement(*f)) {
  // Feature blocks movement
}
```

### Action Implementation Pattern
```cpp
ActionResult execute(Map& map, FeatureManager& features, ...) {
  // 1. Check bounds
  if (!map.inBounds(target)) return Invalid;
  
  // 2. Check terrain blocking
  if (map.blocksMovement(target)) return Failure;
  
  // 3. Check feature blocking
  if (features.blocksMovement(target)) return Failure;
  
  // 4. Check entities
  Entity* e = entities.getEntityAt(target);
  if (e) { /* handle */ }
  
  // 5. Perform action
  // modify features, entities, etc.
  
  return Success;
}
```

## Extension Points

### New Generator Type
1. Create generator in `world/gen/`
2. Add enum to `GeneratorType`
3. Add case in `LevelGenerator::generateTerrain()`
4. Generator should only modify Map (terrain)

### New Action
1. Inherit pattern from MoveAction/OpenAction
2. Accept Map& and FeatureManager& parameters
3. Check terrain, then features, then entities
4. Return ActionResult with energy cost

### New AI Behavior
1. Implement AIBehavior interface
2. Accept FeatureManager& in act() method
3. Create actions and pass features through
4. Return ActionResult

## Testing Checklist

- [ ] Stairs generate (1-3 per level)
- [ ] Stairs are Features, not Tiles
- [ ] Doors auto-place at junctions
- [ ] Player spawns away from stairs
- [ ] Monsters spawn away from stairs
- [ ] Can descend stairs with '>'
- [ ] Can open doors with 'o'
- [ ] Monsters respect closed doors
- [ ] Look mode shows feature info
- [ ] HP preserved between levels
- [ ] Rendering shows all layers correctly

## File Locations

```
src/
  world/
    Map.hpp/cpp                 - Terrain (Floor, Wall)
    FeatureManager.hpp/cpp      - Feature storage
    Feature.hpp                 - Feature types (Door, Stairs)
    FeatureProperties.hpp       - Feature helpers
    gen/
      LevelGenerator.hpp/cpp    - Main orchestrator
      FeaturePlacer.hpp/cpp     - Feature placement
      LevelData.hpp             - Data transfer object
      RoomsGen.cpp              - Rooms+corridors generator
      CavesGen.cpp              - Cellular automata generator
  actions/
    MoveAction.cpp              - Uses Map + Features
    OpenAction.cpp              - Uses Features
  ai/
    AIBehavior.hpp              - Interface with Features
    SimpleAI.cpp                - Uses Map + Features
  Game.cpp                      - Owns Map + FeatureManager
  renderers/
    FTXUIRenderer.cpp           - Renders Map + Features
```

## Common Issues

### "Nothing to open here"
- Solution: Move adjacent to door, then press 'o'

### Monsters walk through closed doors
- Check: MoveAction receives FeatureManager
- Check: AI passes features to actions

### Stairs don't work
- Check: FeatureManager initialized in Game
- Check: descendStairs() checks features
- Check: Stairs are Feature, not Tile

### Features don't render
- Check: GameState has features pointer
- Check: Renderer checks features before tiles
- Check: FeatureManager passed to renderer

## Performance

- Map size: 60x40 = 2400 tiles
- Feature scan: O(tiles) per feature type
- Total generation: <10ms typical
- Feature lookup: O(1) via unordered_map

## Memory

- Map: vector of Tiles (1 byte each) ≈ 2.4KB
- Features: unordered_map (sparse) ≈ varies, typically <1KB
- Total per level: <5KB

## Thread Safety

- Not thread-safe (single-threaded game)
- All generation happens on main thread
- No concurrent access to Map or FeatureManager

## Future Considerations

- Level stack for backtracking
- Connectivity validation
- Multiple generators per level
- Procedural feature generation
- Serialization support
- Multi-threaded generation (pre-generate next level)
