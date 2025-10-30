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
│   │   ├── FOV.cpp                FOV - Bresenham line-of-sight with blocking tile visibility
│   │   ├── FOV.hpp                FOV definitions, depends only on IMapView
│   │   ├── IMapView.hpp           interface providing minimal map access for FOV
│   │   ├── InputAction.hpp        input action enum (separated for avoiding circular deps)
│   │   ├── InputHandler.cpp       keyboard input handling implementation
│   │   ├── InputHandler.hpp       raw input reading and direction conversion
│   │   ├── InputMapper.cpp        key-to-action mapping with scheme support
│   │   ├── InputMapper.hpp        input mapper declarations
│   │   ├── InputScheme.cpp        preset key binding schemes (Vi, WASD, Arrows)
│   │   ├── InputScheme.hpp        scheme definitions and helpers
│   │   ├── Pathfinding.cpp        A* pathfinding algorithm implementation
│   │   ├── Pathfinding.hpp        pathfinding declarations using IMapView interface
│   │   ├── Position.hpp           basic logic for tile positions with operators
│   │   └── Types.hpp              basic types, currently empty
│   ├── entities                   entity system
│   │   ├── Entity.cpp             entity implementation with generic property system and AI
│   │   ├── Entity.hpp             entity class with position, flexible properties, glyph, and AI behavior
│   │   ├── EntityManager.cpp      entity collection management implementation
│   │   ├── EntityManager.hpp      entity manager for adding, removing, finding entities
│   │   ├── TurnManager.cpp        turn-based system implementation (accumulation-based energy)
│   │   └── TurnManager.hpp        energy-based turn order management with speed property
│   ├── Game.cpp                   main game class - orchestrates all systems
│   ├── Game.hpp                   game state, level generation, input handling
│   ├── main.cpp                   entry point - creates Game and runs
│   ├── renderers                  rendering backends
│   │   ├── FTXUIRenderer.cpp      FTXUI-based terminal renderer with panel layout
│   │   └── FTXUIRenderer.hpp      GameState struct and renderer declarations
│   ├── systems                    game logic systems
│   │   ├── CombatSystem.cpp       combat calculations and damage formula
│   │   └── CombatSystem.hpp       combat system declarations
│   └── world                      world building algorithms and logic
│       ├── gen                    generators
│       │   ├── CavesGen.cpp       Cellular Automata module map generator
│       │   ├── CavesGen.hpp       struct CavesOptions and main CA method declaration
│       │   ├── GenOptions.hpp     common generator options base (CommonGenOptions)
│       │   ├── MapGenerator.cpp   main map generator using modules
│       │   ├── MapGenerator.hpp   main map generator definitions (uses forward declarations)
│       │   ├── RoomsGen.cpp       rooms and corridors module map generator with stairs placement
│       │   └── RoomsGen.hpp       struct RoomsOptions and generateRoomsModule method declaration
│       ├── Map.cpp                map routines
│       ├── Map.hpp                map Class and basic methods, includes fill(Tile) utility
│       ├── MapViewAdapter.hpp     adapter between world::Map and core::IMapView
│       └── Tile.hpp               enum describing tiles (Floor, Wall, Doors, Stairs) with helper functions
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
- Part of core namespace for consistency

### IMapView.hpp
- Purpose: minimal interface for map access needed by algorithms
- Pure virtual interface with width(), height(), and blocksLineOfSight()
- Allows FOV and pathfinding to work without depending on full Map class

### FOV (Field of View)
- Implementation: Bresenham line-of-sight algorithm
- Uses IMapView interface for map access
- compute() calculates visible tiles from given position and radius
- isVisible() checks if specific tile is visible
- **Fixed:** Blocking tiles (walls) on FOV edge are now marked as visible
- Method: markRayUntilBlocked() marks all tiles along ray including first blocker

### Pathfinding
- Implementation: A* algorithm
- Uses IMapView interface for map access
- findPath() returns vector of positions from start to goal
- Handles obstacles and finds optimal path

### Input System
**InputAction.hpp** - Separated enum for avoiding circular dependencies
- Actions: Move (8-dir), Wait, Open, Look, Descend, Quit

**InputHandler** - Raw input reading
- readChar() for single character input
- actionToDirection() converts movement actions to Position offsets

**InputMapper** - Configurable key bindings
- Maps raw keys to InputActions
- Runtime scheme switching capability
- Three preset schemes via InputScheme

**InputScheme** - Preset key binding configurations
- Vi scheme: hjkl/yubn movement
- WASD scheme: wasd/qezc movement  
- Arrows scheme: numpad fallback
- Easy to add new schemes without code changes

## 07.02. World Systems

### Tile.hpp
- Enum class Tile: Floor, Wall, DoorClosed, DoorOpen, StairsDown, StairsUp
- Helper functions: isWall(), isFloor(), isDoor(), isStairs(), blocksMovement(), blocksLineOfSight()
- **Design note:** Uses exhaustive switch statements - requires update for each new tile type

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

**RoomsGen**: Rooms and corridors algorithm with stairs placement
- Options: max_rooms, room_min, room_max, add_doors
- Generates rectangular rooms with L-shaped corridors
- Ensures rooms don't overlap (1-tile padding)
- Connects rooms in sorted order (by X coordinate)
- **Stairs placement:** Probability-based selection across rooms
  - First stairs: guaranteed with increasing probability (room_index/total_rooms)
  - Second stairs: 15% chance with same probability distribution
  - Third stairs: 5% chance with same probability distribution
  - Uses anonymous namespace helpers: placeStairsInRoom(), tryPlaceStairsWithProbability()
- **Known issue:** Sequential corridor connection doesn't guarantee full connectivity

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
- Glyph: char for visual representation ('@' player, 'g' goblin, etc.)
- Methods: getProperty(), setProperty(), hasAI(), setAI(), getAI(), getGlyph(), setGlyph()

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

### Descend Action
- Bound to '>' key in all input schemes
- Validates player is standing on StairsDown tile
- Triggers level regeneration via Game::generateLevel()
- Preserves player HP between levels
- Increments depth counter

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
- Creates fresh MapViewAdapter each call to avoid stale references

## 07.06. Combat System

### CombatSystem
- Static methods for combat calculations
- meleeAttack(): calculates damage and applies to defender
- Damage formula: STR × (1 - phys_res)
  - phys_res clamped to [0.0, 0.75]
  - Minimum 1 damage guaranteed
- Returns AttackResult with damage, killed flag, and message
- Updates defender HP directly

## 07.07. Game Architecture

### Game Class
- Encapsulates entire game state and systems
- Owns: Map, FOV, EntityManager, TurnManager, InputMapper, Renderer
- Methods:
  - run(): main game loop
  - handleInput(): processes player actions
  - handleLookMode(): cursor-based tile examination
  - descendStairs(): level transition logic
  - generateLevel(): procedural level generation with entity spawning
  - processPlayerTurn() / processAITurns(): turn processing
- **State management:**
  - messages_: message log with 1000 message limit
  - discoveredTiles_: tracks explored map for minimap
  - depth_: current dungeon depth
  - turnCounter_: game turn tracking

### Level Generation (Game::generateLevel)
- Procedural generation using MapGenerator
- Random seed per level (std::random_device)
- Finds valid spawn position (floor tile, not stairs)
- Preserves player HP across levels
- Spawns 20 goblins with SimpleAI
- Resets FOV and discovered tiles
- **Known issue:** No validation for stairs reachability (disconnected rooms possible)

## 07.08. Rendering System

### FTXUIRenderer
- Professional TUI rendering using FTXUI library
- Fixed 80x24 terminal size (classic roguelike)
- Panel-based layout using FTXUI flexbox system:
  - Status bar (depth, turn counter)
  - Game view (50x19) with FOV-based visibility
  - Message log (30x8) with scrolling
  - Look info panel (30x6) - active during examine mode
  - Minimap (30x5) - discovered area visualization
  - Stats bar (player name, HP bar, help hint)

### GameState struct
- Data transfer object for renderer
- Contains non-owning pointers to game state
- Fields: map, fov, entities, player, camera, cursor, messages, UI state, stats

### Rendering Features
- Tile glyphs: '.', '#', '+', '\'', '>', '<', '@', 'g'
- FOV-based visibility (unseen = space)
- Cursor visualization in look mode ('X')
- HP bar visualization with filled/empty segments
- Word-wrapped messages with automatic scrolling
- Minimap with scaled representation and player marker
- Border separators between panels

## 07.09. Look Mode System

### Features
- Activated with 'x' key
- Cursor movement restricted to visible tiles
- Displays tile information: type, distance, entity details
- Info panel shows:
  - Distance from player (Chebyshev distance)
  - Tile type (Floor, Wall, Door status)
  - Entity at position (name, HP)
- Exit with 'x' or 'q'
- Visual feedback: cursor position shown as 'X' on map

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

08.04. Dependency inversion for action-manager relationship
- Current: MoveAction includes TurnManager to remove killed entities
- Problem: Low-level action depends on high-level manager (inverted hierarchy)
- Solutions:
  - A) Return entity_to_remove in ActionResult, handle in Game loop
  - B) Event system with publish/subscribe
  - C) Observer pattern - EntityManager notifies TurnManager
- Priority: medium - works but architecturally imperfect
- Recommendation: Option A for simplicity

08.05. Tile properties as data, not code
- Current: blocksMovement/blocksLOS use exhaustive switch statements
- Problem: Must update every helper function when adding new tile type
- Improvement: TileProperties struct with table lookup
- Benefits: easier to add tiles, less error-prone, data-driven
- Priority: medium - becomes important when adding many tile types

08.06. Level connectivity validation
- Current: RoomsGen connects rooms sequentially (sorted by X)
- Problem: Doesn't guarantee all rooms are reachable (stairs may be isolated)
- Solutions:
  - A) Flood fill validation + regenerate if stairs unreachable
  - B) Minimum Spanning Tree for corridor generation
  - C) Better corridor algorithm ensuring full connectivity
- Priority: medium - affects gameplay quality
- Current workaround: Multiple stairs increase chances of reachable exit

# 09. Architecture decisions and lessons learned

09.01. **Action-based architecture**
- All game actions (move, open, attack, descend) are objects implementing execute()
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
- core/: reusable algorithms (FOV, pathfinding, input)
- actions/: game-specific behavior
- systems/: game logic (combat, future: inventory, etc.)
- ai/: entity decision making
- Benefits: clear dependencies, easier to understand and maintain

09.05. **Ownership and memory management**
- EntityManager owns entities via unique_ptr
- TurnManager stores raw pointers (non-owning)
- Critical: when entity dies, remove from ALL managers
- Lesson: clear ownership rules prevent memory bugs and dangling pointers
- Descend stairs must preserve player HP before destroying old EntityManager

09.06. **Configuration centralization**
- All magic numbers moved to config/DungeonConfig.hpp
- Presets for common scenarios
- Benefits: easy tuning, no hunting through code for values
- Lesson: separate "what" (config) from "how" (implementation)

09.07. **FTXUI for professional TUI**
- Lesson: Manual ANSI escape codes are error-prone and limited
- FTXUI provides: automatic layout, borders, flexbox, panels
- Benefits: clean code, maintainable UI, professional appearance
- Architecture: GameState struct decouples game logic from rendering
- Fixed 80x24 size maintains classic roguelike aesthetic

09.08. **Input system modularity**
- Separated concerns: InputHandler (raw input), InputMapper (key→action), InputScheme (presets)
- Benefits: runtime scheme switching, easy to add new schemes, no hardcoded keys
- Lesson: Separating InputAction enum avoids circular dependencies

09.09. **Message system with history**
- 1000 message limit prevents unbounded memory growth
- Renderer shows last N that fit (with word wrap)
- Ready for future: full-screen message log viewer
- Helper method (addMessage) centralizes limit enforcement

09.10. **Procedural generation challenges**
- Lesson: Random generation can produce edge cases (disconnected rooms, no stairs)
- Current approach: Probability-based stairs placement increases success rate
- Future improvement: Validation + regeneration or better connectivity algorithm
- Trade-off: Perfect generation vs. development time (MVP accepts rare failures)

09.11. **FOV edge case handling**
- Initial implementation: blocking tiles were invisible
- Fix: Mark all tiles along ray including first blocker
- Lesson: Edge cases in algorithms often discovered through playtesting
- Method: markRayUntilBlocked() provides intuitive FOV behavior

09.12. **Procedural approach for generators**
- Choice: Procedural functions vs. class-based generators
- Current: Procedural with anonymous namespace helpers
- Benefits: Simple, direct, less boilerplate
- Trade-off: Harder to preserve intermediate state (rooms vector)
- Lesson: Choose simplest approach that works, refactor when complexity demands it
