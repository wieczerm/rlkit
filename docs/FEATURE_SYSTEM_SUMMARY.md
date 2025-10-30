# Feature System Implementation Summary

## Files Created

### Core Implementation
1. **src/world/Feature.hpp** - Feature types (Door, Stairs) and variant
2. **src/world/FeatureProperties.hpp** - Property query functions (header-only)
3. **src/world/FeatureManager.hpp** - Manager interface
4. **src/world/FeatureManager.cpp** - Manager implementation

### Testing
5. **tests/FeatureTests.cpp** - Comprehensive test suite (16 tests)

## Feature Types Implemented

### Door
```cpp
struct Door {
    enum class Material { Wood, Iron, Stone };
    enum class State { Open, Closed };  // Locked state: future (needs inventory)
    
    Material material;
    State state;
};
```

**Properties:**
- Open door: doesn't block movement/LOS, glyph '\''
- Closed door: blocks movement/LOS, glyph '+'
- Future: Locked state (requires key from inventory)

### Stairs
```cpp
struct Stairs {
    enum class Direction { Down, Up };
    
    Direction direction;
    int target_depth;
};
```

**Properties:**
- Never blocks movement or LOS
- Down stairs: glyph '>'
- Up stairs: glyph '<'

## FeatureManager API

### Storage (one feature per tile)
```cpp
void addFeature(Position pos, Feature feature);     // Add or replace
void removeFeature(Position pos);                   // Remove if exists
bool hasFeature(Position pos) const;                // Check existence
Feature* getFeature(Position pos);                  // Get for modification
```

### Property Queries
```cpp
bool blocksMovement(Position pos) const;            // Depends on feature state
bool blocksLineOfSight(Position pos) const;         // Depends on feature state
```

### Utilities
```cpp
void clear();                                       // Remove all features
std::vector<Position> getAllPositions() const;      // Get all feature locations
size_t size() const;                                // Count features
```

## Property Functions (FeatureProperties.hpp)

Dynamic queries based on feature state:
```cpp
bool blocksMovement(const Feature& f);
bool blocksLineOfSight(const Feature& f);
bool isInteractable(const Feature& f);
char getGlyph(const Feature& f);

// Type checking
bool isDoor(const Feature& f);
bool isStairs(const Feature& f);

// Safe type extraction
Door* getDoor(Feature& f);             // nullptr if not a door
Stairs* getStairs(Feature& f);         // nullptr if not stairs
```

## Integration with Existing Systems

### OpenAction (Door Interaction)
```cpp
ActionResult OpenAction::execute(Game& game) {
    Feature* feature = game.featureManager().getFeature(target_pos);
    if (!feature) return invalid("Nothing to open here");
    
    Door* door = getDoor(*feature);
    if (!door) return invalid("Can't open that");
    
    if (door->state == Door::State::Open) {
        return invalid("Door is already open");
    }
    
    // Open the door
    door->state = Door::State::Open;
    
    // Push event
    game.eventQueue().push(DoorOpenedEvent{target_pos});
    
    return success("You open the door", ENERGY_COST);
}
```

### DescendAction (Stairs Interaction)
```cpp
ActionResult DescendAction::execute(Game& game) {
    Position player_pos = game.player()->position();
    
    // Check for stairs feature
    Feature* feature = game.featureManager().getFeature(player_pos);
    if (!feature) return invalid("No stairs here");
    
    Stairs* stairs = getStairs(*feature);
    if (!stairs) return invalid("No stairs here");
    
    if (stairs->direction != Stairs::Direction::Down) {
        return invalid("These stairs go up");
    }
    
    // Trigger level transition
    game.eventQueue().push(LevelTransitionEvent{
        LevelTransitionEvent::Down,
        game.depth(),
        stairs->target_depth,
        player_pos
    });
    
    return success("You descend the stairs", ENERGY_COST);
}
```

### Map and FOV Integration
```cpp
// Map::blocksMovement needs to check both tiles AND features
bool Map::blocksMovement(Position pos) const {
    // Check tile
    if (world::blocksMovement(at(pos))) return true;
    
    // Check feature
    if (featureManager_.blocksMovement(pos)) return true;
    
    return false;
}

// Same for blocksLineOfSight
bool Map::blocksLineOfSight(Position pos) const {
    if (world::blocksLineOfSight(at(pos))) return true;
    if (featureManager_.blocksLineOfSight(pos)) return true;
    return false;
}
```

### Renderer Integration
```cpp
char getGlyphAt(Position pos) const {
    // Priority: Entity > Feature > Tile
    
    if (Entity* entity = entityManager.getEntityAt(pos)) {
        return entity->getGlyph();
    }
    
    if (Feature* feature = featureManager.getFeature(pos)) {
        return getGlyph(*feature);
    }
    
    return getTileGlyph(map.at(pos));
}
```

## Door Events

Need to add to Event.hpp:
```cpp
struct DoorOpenedEvent {
    static constexpr EventPriority priority = EventPriority::Normal;
    Position position;
    Door::Material material;
};

struct DoorClosedEvent {
    static constexpr EventPriority priority = EventPriority::Normal;
    Position position;
    Door::Material material;
};
```

Then add to Event variant:
```cpp
using Event = std::variant<
    EntityDiedEvent,
    EntityDamagedEvent,
    EntityMovedEvent,
    LevelTransitionEvent,
    DoorOpenedEvent,      // NEW
    DoorClosedEvent,      // NEW
    TestEvent
>;
```

## CMakeLists.txt Changes

Add to WORLD_SOURCES:
```cmake
src/world/FeatureManager.cpp
```

Add to WORLD_HEADERS:
```cmake
src/world/Feature.hpp
src/world/FeatureProperties.hpp
src/world/FeatureManager.hpp
```

Add test:
```cmake
add_executable(feature_tests tests/FeatureTests.cpp)
target_link_libraries(feature_tests PRIVATE core world)
add_test(NAME feature_tests COMMAND feature_tests)
```

## Test Coverage

✅ Door states (Open, Closed) and materials
✅ Stairs directions (Down, Up) and target depths
✅ Feature variant type safety
✅ Dynamic property queries (blocksMovement, blocksLOS)
✅ Glyph rendering per feature type and state
✅ Type checking and safe extraction helpers
✅ FeatureManager add/remove/query operations
✅ Property delegation through manager
✅ Feature replacement at same position
✅ getAllPositions for iteration
✅ Door state transitions (open/close cycle)

## Design Notes

### Position as Identifier
- Features identified by position (no separate IDs)
- Sufficient for static features
- Can add IDs later if needed (quests, scripting)

### Action-Driven Interaction
- Features are passive data
- Actions modify features and push events
- Keeps feature system simple

### Future Extensions

**Locked Doors** (requires inventory system):
```cpp
enum class State { Open, Closed, Locked };
int key_id;  // Item ID required to unlock
```

**Additional Feature Types:**
```cpp
struct Chest {
    bool is_open;
    bool is_locked;
    int key_id;
    std::vector<int> item_ids;
};

struct Trap {
    enum class Type { Spike, Poison, Teleport };
    Type type;
    bool is_triggered;
    int damage;
};
```

## Testing Instructions

```bash
# Compile
g++ -std=c++20 -I. -Isrc src/world/FeatureManager.cpp \
    tests/FeatureTests.cpp -o feature_tests

# Run
./feature_tests

# Expected output:
# === Feature System Tests ===
# Testing Door states...
#   ✓ Door states work correctly
# ...
# ✓ All tests passed!
```

## Integration Checklist

Before using Feature system in Game:
- [ ] Add DoorOpenedEvent/DoorClosedEvent to Event.hpp
- [ ] Update Map to check features for blocking (movement/LOS)
- [ ] Update OpenAction to work with FeatureManager
- [ ] Update DescendAction to work with FeatureManager  
- [ ] Update renderer to display feature glyphs
- [ ] Update generators to place Door and Stairs features (not tiles)
- [ ] Remove DoorClosed/DoorOpen/StairsUp/StairsDown from Tile enum
- [ ] Test full integration

## Next Steps

1. Test Feature system independently
2. Add door events to Event system
3. Design Serialization layer (next in sequence)
4. Then: migrate Map and generators to use Features
