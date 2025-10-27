# Rogueline Engine - design documentation

# 01. Project description
Building C++ roguelike engine with following characteristics:
- modular - careful design choices as we are adding more functionalities
- minimal 
- scalable

# 02. Project current goals
- learning OOP i C++
- learinng proper classes design
- learning proper project and dependancies design
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
│   │   ├── Entity.cpp             entity implementation with generic property system
│   │   ├── Entity.hpp             entity class with position and flexible properties
│   │   ├── EntityManager.cpp     entity collection management implementation
│   │   ├── EntityManager.hpp     entity manager for adding, removing, finding entities
│   │   ├── TurnManager.cpp       turn-based system implementation
│   │   └── TurnManager.hpp       energy-based turn order management
│   ├── main.cpp                   demo game with movement, FOV, and entities
│   ├── renderers                  rendering backends
│   │   ├── FTXUIRenderer.cpp     FTXUI terminal renderer implementation
│   │   └── FTXUIRenderer.hpp     terminal rendering declarations
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
│       ├── Map.hpp                map Class and basic methods, includes fill(Tile) utility replacing reset_to_walls
│       ├── MapViewAdapter.hpp     adapter between world::Map and core::IMapView
│       └── Tile.hpp               struct describing single tile and some basic methods
└── tests                          storing test files 
    ├── EntityManagerTests.cpp     testing entity manager functionality
    ├── EntityTests.cpp            testing entity system and properties
    ├── FOVTests.cpp               testing FOV implementation
    ├── GenDoorsTest.cpp           testing door placement implementation
    ├── GenTests.cpp               generator test
    ├── include
    │   └── assertions.hpp
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
07.01. Position.hpp
- purpose: to define most basic coordinates of the tile
- includes: none
- defines struct Position with int x, y, constructors Position() and Position(int, int)
- overloads operators ==, !=, +, -

07.02. Types.hpp
- purpose: define most basic types if necessary
- state: currently is empty

07.03. IMapView.hpp
- purpose: define minimal interface for map-like structures usable by core algorithms
- includes: none
- defines struct IMapView with pure virtual methods width(), height(), blocksLos(int,int)
- namespace: core

07.04. FOV.hpp
- purpose: declarations for FOV algorithm implementation
- includes: "IMapView.hpp", "Position.hpp", <vector>
- defines class FOV with constructor explicit FOV(const core::IMapView &map);
- methods definitions:
    public:
      void compute(const Position &origin, int radius);
      bool isVisible(int x, int y) const;
    private:
      bool hasLineOfSight(int x0, int y0, int x1, int y1) const;
- state: functional, depends on IMapView, no longer includes world/Map.hpp

07.05. FOV.cpp
- purpose: Bresenham line-of-sight FOV algorithm
- includes: "FOV.hpp", <algorithm>, <cmath>
- methods implemented: FOV::FOV, FOV::compute, FOV::isVisible, FOV::hasLineOfSight
- algorithm: simple Bresenham-based line-of-sight checks within radius
- map access through IMapView::blocksLos()
- state: stable and reliable

07.06. Pathfinding.hpp
- purpose: declarations for A* pathfinding algorithm
- includes: "IMapView.hpp", "Position.hpp", <vector>
- namespace: core
- defines class Pathfinding with constructor explicit Pathfinding(const IMapView &map)
- methods:
    findPath(const Position &start, const Position &goal) - returns vector of positions
    heuristic(const Position &a, const Position &b) - Manhattan distance
    getNeighbors(const Position &pos) - returns 8-directional walkable neighbors

07.07. Pathfinding.cpp
- purpose: A* pathfinding implementation
- includes: "Pathfinding.hpp", <algorithm>, <cmath>, <queue>, <unordered_map>, <unordered_set>
- algorithm: A* with priority queue, Manhattan heuristic, 8-directional movement
- uses IMapView::blocksLos() to check walkability
- state: functional and tested

07.08. InputHandler.hpp
- purpose: keyboard input handling declarations
- includes: "Position.hpp"
- namespace: core
- defines enum class InputAction with movement directions, Wait, Quit, None
- defines class InputHandler with methods:
    getAction() - blocking keyboard input
    static actionToDirection(InputAction) - converts action to Position delta

07.09. InputHandler.cpp
- purpose: keyboard input implementation
- includes: "InputHandler.hpp", <iostream>, <termios.h>, <unistd.h>
- supports vi keys (hjkl yubn), WASD, wait (. or space), quit (q)
- uses terminal raw mode for immediate input without buffering
- state: functional

07.10. Entity.hpp
- purpose: define entity class with flexible property system
- includes: "../core/Position.hpp", <string>, <unordered_map>
- namespace: entities
- defines class Entity with:
    position management (getPosition, setPosition)
    name (getName)
    generic property system (setProperty, getProperty, hasProperty)
    convenience helpers (setHP, getHP, setMaxHP, getMaxHP)
- properties stored as std::unordered_map<std::string, int>

07.11. Entity.cpp
- purpose: entity implementation
- includes: "Entity.hpp"
- methods implemented: constructor, setProperty, getProperty, hasProperty
- state: functional

07.12. EntityManager.hpp
- purpose: manage collection of entities
- includes: "Entity.hpp", "../core/Position.hpp", <memory>, <vector>
- namespace: entities
- defines class EntityManager with methods:
    addEntity(std::unique_ptr<Entity>) - takes ownership
    removeEntity(Entity*) - removes by pointer
    getEntityAt(const Position&) - returns first entity at position
    getEntitiesAt(const Position&) - returns all entities at position
    getEntities() - returns all entities
    clear() - removes all
    count() - returns entity count

07.13. EntityManager.cpp
- purpose: entity manager implementation
- includes: "EntityManager.hpp", <algorithm>
- uses smart pointers for ownership management
- state: functional and tested

07.14. TurnManager.hpp
- purpose: turn-based gameplay system
- includes: "Entity.hpp", <queue>, <vector>
- namespace: entities
- defines struct TurnEntry with entity pointer and energy
- defines class TurnManager with methods:
    addEntity(Entity*) - adds to turn queue using "speed" property
    removeEntity(Entity*) - removes from queue
    getNextActor() - returns next entity to act
    processTurn() - advances time and returns acting entity
    isEmpty() - checks if queue empty
    clear() - removes all entities

07.15. TurnManager.cpp
- purpose: energy-based turn system implementation
- includes: "TurnManager.hpp", <algorithm>
- uses priority queue, each turn costs 100 energy
- faster entities (higher speed property) act more frequently
- state: functional and tested

07.16. FTXUIRenderer.hpp
- purpose: terminal rendering declarations
- includes: "../world/Map.hpp", "../entities/EntityManager.hpp", "../core/FOV.hpp", <string>
- namespace: renderers
- defines class FTXUIRenderer with methods:
    render(map, fov, entityMgr, cameraCenter) - renders game world
    renderMessages(messages) - renders message log
    renderStats(name, hp, maxHp) - renders entity stats
    processFrame() - frame processing
    tileToChar(Tile) - converts tile to ASCII character

07.17. FTXUIRenderer.cpp
- purpose: FTXUI renderer implementation
- includes: "FTXUIRenderer.hpp", "../world/Tile.hpp", <ftxui/dom/elements.hpp>, <ftxui/screen/screen.hpp>, <iostream>
- currently uses simple cout output
- tile mapping: Floor(.), Wall(#), DoorClosed(+), DoorOpen(/), Entity(@)
- camera follows player with configurable viewport size
- state: basic functionality working

07.18. Tile.hpp
- purpose: to describe a single tile on a map and define checking methods
- includes: <cstdint>
- declarations in namespace world
- class definition: enum class Tile, currently with Floor, Wall, DoorClosed, DoorOpen
- methods:
    blocksMove(Tile) - returns true for Wall and DoorClosed
    blocksLos(Tile) - returns true for Wall and DoorClosed
    isDoor(Tile)
    isWall(Tile)
    isFloor(Tile)
    isDoorClosed(Tile)
    isDoorOpen(Tile)
- state: fixed blocksMove implementation

07.19. Map.hpp
- purpose: define class Map for use with map generator
- includes: "../core/Position.hpp", "Tile.hpp", <cassert>, <cstddef>, <vector>
- declarations in namespace world
- methods:
    width()
    height()
    inBounds(Position)
    inBounds(int, int)
    at(Position)
    set(Position, Tile)
    isOpaque(int,int)
    blocksMove(Position)
    blocksLos(Position)
    fill(Tile) - replaces reset_to_walls, fills map with provided Tile
    private idx(Position)

07.20. Map.cpp
- purpose: map routines
- state: currently minimal

07.21. MapViewAdapter.hpp
- purpose: adapter between world::Map and core::IMapView
- includes: "Map.hpp", "../core/IMapView.hpp"
- namespace: world
- implements IMapView interface for Map class
- allows core algorithms to work with world::Map through interface

07.22. GenOptions.hpp
- purpose: contain shared CommonGenOptions base struct for all generators
- includes: none
- defines struct CommonGenOptions with field add_doors = true;

07.23. CavesGen.hpp
- purpose: definitions for Cellular Automata caves generator module
- includes: "GenOptions.hpp", "../Map.hpp", <random>
- declarations in namespace world
- declares:
    struct CavesOptions : CommonGenOptions
        fill_percent = 52
        steps = 5
        birth = 5
        survive = 4
    method generateCavesModule(Map, CavesOptions, std::mt19937)

07.24. CavesGen.cpp
- purpose: Cellular Automata caves floor module generator implementation
- state: functional

07.25. RoomsGen.hpp
- purpose: rooms and corridors generator declarations
- includes: "GenOptions.hpp", "../Map.hpp", <random>
- namespace: world
- declares struct RoomsOptions : CommonGenOptions with room_min, room_max, max_rooms
- method generateRoomsModule(Map, RoomsOptions, std::mt19937)

07.26. RoomsGen.cpp
- purpose: rooms and corridors generator implementation with door placement
- state: functional with door placement working

07.27. MapGenerator.hpp
- purpose: main map generator using modules
- includes: "../Map.hpp", <random>
- namespace: world
- class MapGenerator with seed-based RNG
- methods: generateRooms(Map, RoomsOptions), generateCaves(Map, CavesOptions)

07.28. MapGenerator.cpp
- purpose: map generator implementation
- coordinates generation modules
- state: functional

07.29. main.cpp
- purpose: playable demo game
- includes: all engine components
- features:
    - map generation with rooms and doors
    - player entity with movement (WASD/vi keys)
    - 5 goblin entities
    - FOV system (radius 8)
    - collision detection
    - message log
    - stats display
    - game loop with input handling
    - quit functionality (Q key)
- state: fully functional demo
