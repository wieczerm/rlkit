# Rogueline Engine - design documentation

# 01. Project description
Building C++ roguelike engine with following characteristics:
- modular - careful design choices as we are adding more functionalities
- minimal 
- scalable

# 02. Project current goals
- learning OOP and C++
- learning proper classes design
- learning proper project and dependencies design
- understanding algorithms used and understanding their implementation

# 03. Project implementation goals
- generation algorithms
- pathfinding
- FOV
- entities and their stats
- entities AI programming through scripting (.json, maybe LUA)
- interactive objects implementation - usable and world objects
- UI
- renderers: FTXUI and SDL

# 04. Future objectives
- build a small roguelike game on the engine
- revisit code to ensure portability (Linux, Windows, MacOS, *BSD)
- extend engine's functionality by adding more algorithms
- unicode tilesets

# 05. Catalogues structure with catalogues and files brief description
.
├── build                          build files, executable demo file and executable algorithm test files
├── CMakeLists.txt
├── DESIGN.md                      this file
├── src                            source files root
│   ├── actions                    action system - game actions (move, open, etc.)
│   │   ├── ActionResult.hpp       result structure for action execution (success/failure/invalid)
│   │   ├── MoveAction.cpp         movement and bump-to-attack action implementation
│   │   ├── MoveAction.hpp         movement action declarations
│   │   ├── OpenAction.cpp         door opening action implementation
│   │   └── OpenAction.hpp         door opening action declarations
│   ├── ai                         AI behavior system
│   │   ├── AIBehavior.hpp         AI interface - all AI types implement this
│   │   ├── SimpleAI.cpp           basic chase AI - pathfinds towards player when visible
│   │   └── SimpleAI.hpp           simple AI declarations
│   ├── config                     configuration and tuning parameters
│   │   └── DungeonConfig.hpp      level configurations, presets, generation parameters
│   ├── core                       core/low-level setup, types and algorithms
│   │   ├── FOV.cpp                FOV - Bresenham line-of-sight algorithm using IMapView interface
│   │   ├── FOV.hpp                FOV definitions, depends only on IMapView
│   │   ├── IMapView.hpp           interface providing minimal map access for FOV
│   │   ├── InputHandler.cpp      keyboard input handling implementation
│   │   ├── InputHandler.hpp      input action definitions and direction conversion
│   │   ├── Pathfinding.cpp       A* pathfinding algorithm implementation
│   │   ├── Pathfinding.hpp       pathfinding declarations using IMapView interface
│   │   ├── Position.hpp           basic logic for tile positions
│   │   └── Types.hpp              basic types, currently empty
│   ├── entities                   entity system
│   │   ├── Entity.cpp             entity implementation with generic property system and AI
│   │   ├── Entity.hpp             entity class with position, flexible properties, and AI behavior
│   │   ├── EntityManager.cpp     entity collection management implementation
│   │   ├── EntityManager.hpp     entity manager for adding, removing, finding entities
│   │   ├── TurnManager.cpp       turn-based system implementation (accumulation-based energy)
│   │   └── TurnManager.hpp       energy-based turn order management with speed property
│   ├── main.cpp                   demo game with movement, FOV, entities, AI, and combat
│   ├── renderers                  rendering backends
│   │   ├── FTXUIRenderer.cpp     FTXUI terminal renderer implementation
│   │   └── FTXUIRenderer.hpp     terminal rendering declarations
│   ├── systems                    game logic systems
│   │   ├── CombatSystem.cpp      combat calculations and damage formula
│   │   └── CombatSystem.hpp      combat system declarations
│   └── world                      world building algorithms and logic
│       ├── gen                    generators
│       │   ├── CavesGen.cpp       Cellular Automata module map generator
│       │   ├── CavesGen.hpp       struct CavesOptions and main CA method declaration
│       │   ├── GenOptions.hpp     common generator options base (CommonGenOptions)
│       │   ├── MapGenerator.cpp   main map generator using modules
│       │   ├── MapGenerator.hpp   main map generator definitions (uses forward declarations)
│       │   ├── RoomsGen.cpp       rooms and corridors module map generator
│       │   └── RoomsGen.hpp       struct RoomsOptions and generateRoomsModule method declaration
│       ├── Map.cpp                map routines
│       ├── Map.hpp                map Class and basic methods, includes fill(Tile) utility
│       ├── MapViewAdapter.hpp     adapter between world::Map and core::IMapView
│       └── Tile.hpp               struct describing single tile and some basic methods
└── tests                          storing test files 
    ├── EntityManagerTests.cpp     testing entity manager functionality
    ├── EntityTests.cpp            testing entity system and properties
    ├── FOVTests.cpp               testing FOV implementation
    ├── GenDoorsTest.cpp           testing door placement implementation
    ├── GenTests.cpp               generator test
    ├── include
    │   └── assertions.hpp         custom test assertion macros
    ├── MapTests.cpp               map generation testing
    ├── PathfindingTests.cpp       testing A* pathfinding
    └── TurnManagerTests.cpp       testing turn-based system

# 06. Code principles: how to generate code
- camelCase naming convention (refactored: resetToWalls, areaIsAllWalls, digToEdgeH/V, etc.)
- local includes with relative path
- provide comments for methods if they are not self-explanatory
- all comments in english
- using C++ 20 standard
- if unsure about new method implementation ask if it was not already implemented elsewhere to avoid duplication
- if there is a good opportunity make suggestions for which external libraries to use to streamline the work

# 07. Implementation progress: what was implemented and what does it do

## 07.01. Core Systems

### Position.hpp
- Purpose: define most basic coordinates of the tile
- Defines struct Position with int x, y, constructors Position() and Position(int, int)
- Operator overloads for addition and equality comparison

### IMapView.hpp
- Purpose: minimal interface for map access needed by algorithms
- Pure virtual interface with width(), height(), and blocksLineOfSight()
- Allows FOV and pathfinding to work without depending on full Map class

### FOV (Field of View)
- Implementation: Bresenham line-of-sight algorithm
- Uses IMapView interface for map access
- compute() calculates visible tiles from given position and radius
- isVisible() checks if specific tile is visible

### Pathfinding
- Implementation: A* algorithm
- Uses IMapView interface for map access
- findPath() returns vector of positions from start to goal
- Handles obstacles and finds optimal path

### InputHandler
- Handles keyboard input for game actions
- Converts raw input to InputAction enum (Move, Open, Wait, Quit)
- actionToDirection() converts movement actions to Position offsets

## 07.02. World Systems

### Tile.hpp
- Enum class Tile: Floor, Wall, DoorClosed, DoorOpen
- Helper functions: isWall(), isFloor(), isDoor(), blocksMovement(), blocksLineOfSight()

### Map.hpp & Map.cpp
- 2D tile grid with width/height
- Constructor: Map(width, height, fillTile)
- Methods: inBounds(), at(), set(), fill(), blocksMovement(), blocksLineOfSight()
- Uses vector<Tile> for storage with row-major indexing

### MapViewAdapter.hpp
- Adapter pattern: converts Map to IMapView interface
- Allows Map to be used with FOV and Pathfinding algorithms

### Map Generators
**MapGenerator**: Main generator coordinating different generation modules

**RoomsGen**: Rooms and corridors algorithm
- Options: max_rooms, room_min, room_max, add_doors
- Generates rectangular rooms with L-shaped corridors
- Ensures rooms don't overlap (1-tile padding)
- Connects rooms in sorted order

**CavesGen**: Cellular Automata cave generation
- Options: fill_percent, steps, birth, survive, add_doors
- Random initial state with CA iterations
- Connects disconnected regions automatically
- Creates organic cave-like structures

**Configuration System** (config/DungeonConfig.hpp):
- Centralized parameter configuration
- Presets: tinyDungeon(), standardDungeon(), largeDungeon(), denseCaves(), tightCaves(), mixedLevel()
- RoomPlacementConfig: attempts_multiplier, attempts_per_room, room_padding, edge_margin
- CaveGenerationConfig: random_percent_min, random_percent_max
- Eliminates magic numbers from generator code

## 07.03. Entity Systems

### Entity.hpp & Entity.cpp
- Generic entity with name, position, and flexible property system
- Properties: unordered_map<string, int> for flexible attributes (hp, str, speed, etc.)
- AI integration: unique_ptr<AIBehavior> for pluggable AI behaviors
- Methods: getProperty(), setProperty(), hasAI(), setAI(), getAI()

### EntityManager.hpp & EntityManager.cpp
- Manages collection of entities using vector<unique_ptr<Entity>>
- addEntity(): takes ownership via move semantics
- removeEntity(): uses erase-remove idiom to safely remove
- getEntityAt(): returns raw pointer to entity at position (or nullptr)
- getEntitiesAt(): returns all entities at position

### TurnManager.hpp & TurnManager.cpp
- **Accumulation-based energy system** for turn order
- Entities start with energy = 0
- Each entity gains energy = speed property per time advance
- Entity acts when energy >= 100, then loses 100 energy
- Uses priority_queue sorted by energy (highest first)
- getNextActor(): advances time if no one ready (all gain speed)
- processTurn(): deducts 100 energy and re-queues entity
- Result: faster entities (higher speed) act more frequently

## 07.04. Action System

### ActionResult.hpp
- Result structure for all actions
- Fields: status (Success/Failure/Invalid), message, energy_cost
- Static factories: success(), failure(), invalid()

### MoveAction
- Handles movement and bump-to-attack
- Validates bounds and terrain blocking
- If target tile has entity: triggers combat via CombatSystem
- If entity killed: removes from EntityManager AND TurnManager
- If tile empty: moves actor to new position

### OpenAction
- Handles door opening
- Checks adjacent tiles for doors
- Changes DoorClosed to DoorOpen
- Returns appropriate success/failure messages

## 07.05. AI System

### AIBehavior.hpp
- Pure virtual interface for all AI types
- act() method takes entity, player, map, entities, turnMgr
- Returns ActionResult after deciding and executing action
- Strategy pattern: different AI types implement different behaviors

### SimpleAI
- Basic chase AI implementation
- Uses FOV to check if player is visible (configurable vision_range)
- If player visible: uses A* pathfinding to move towards player
- If player not visible: waits (returns success with no action)
- Bump attacks player automatically via MoveAction

## 07.06. Combat System

### CombatSystem
- Static methods for combat calculations
- meleeAttack(): calculates damage and applies to defender
- Damage formula: STR × (1 - phys_res)
  - phys_res clamped to [0.0, 0.75]
  - Minimum 1 damage guaranteed
- Returns AttackResult with damage, killed flag, and message
- Updates defender HP directly

## 07.07. Rendering

### FTXUIRenderer
- Terminal-based renderer using FTXUI library
- render(): displays map, entities, FOV-based visibility
- renderStats(): shows entity name, HP bar
- renderMessages(): displays game messages
- tileToChar(): converts Tile enum to display character

# 08. Future tweaks
Ideas for potential improvements - not critical, implement only when needed (YAGNI principle):

08.01. Type-safe Entity property system
- Current limitation: properties are unordered_map<string, int> - only integers supported
- Improvement: use std::variant to support multiple types (int, float, string, bool)
- Benefits: type safety, autocomplete, runtime flexibility
- Priority: low - current system works fine for basic needs

08.02. Strategy pattern for generators
- Current limitation: MapGenerator hardcoded to RoomsGen and CavesGen
- Improvement: plugin architecture with generator registration
- Benefits: add new generators without modifying existing code, easier testing
- Priority: low - only needed if planning multiple new generator types
- YAGNI: implement only when actually adding 3+ different generators

08.03. Professional testing framework
- Current: custom assertion macros (EXPECT_TRUE, EXPECT_EQ) in tests/include/assertions.hpp
- Improvement: integrate Catch2 or Google Test
- Benefits: better test reporting, fixtures, parameterized tests, industry-standard tools
- Priority: low - current system works fine, good learning opportunity for C++ testing
- YAGNI: implement when test complexity increases or need advanced features

# 09. Architecture decisions and lessons learned

09.01. **Action-based architecture**
- All game actions (move, open, attack) are objects implementing execute()
- Benefits: consistent interface, easy to add new actions, testable
- Actions handle their own validation and side effects

09.02. **Strategy pattern for AI**
- AI behaviors are pluggable via AIBehavior interface
- Each entity can have different AI or none
- Benefits: reusable AI types, runtime swapping, easy to extend
- Enables future scripting (Lua) integration

09.03. **Accumulation-based energy system**
- Entities accumulate energy based on speed property
- Act when energy >= threshold (100)
- Benefits: naturally handles variable speeds, fair turn distribution
- Lesson: cost-based systems without time advancement can starve slow entities

09.04. **Separation of concerns**
- core/: reusable algorithms (FOV, pathfinding)
- actions/: game-specific behavior
- systems/: game logic (combat, future: inventory, etc.)
- ai/: entity decision making
- Benefits: clear dependencies, easier to understand and maintain

09.05. **Ownership and memory management**
- EntityManager owns entities via unique_ptr
- TurnManager stores raw pointers (non-owning)
- Critical: when entity dies, remove from ALL managers
- Lesson: clear ownership rules prevent memory bugs and dangling pointers

09.06. **Configuration centralization**
- All magic numbers moved to config/DungeonConfig.hpp
- Presets for common scenarios
- Benefits: easy tuning, no hunting through code for values
- Lesson: separate "what" (config) from "how" (implementation)
