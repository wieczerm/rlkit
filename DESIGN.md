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
│   │   ├── FOV.cpp                FOV - Chebyshev algorithm using IMapView interface
│   │   ├── FOV.hpp                FOV definitions, depends only on IMapView
│   │   ├── IMapView.hpp           interface providing minimal map access for FOV
│   │   ├── Position.hpp           basic logic for tile positions
│   │   └── Types.hpp              basic types, currently empty
│   ├── main.cpp                   demo file for engine testing
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
    ├── FOVTests.cpp               testing FOV implementation
    ├── GenDoorsTest.cpp           testing door placement implementation
    ├── GenTests.cpp               generator test
    ├── include
    │   └── assertions.hpp
    └── MapTests.cpp               map generation testing

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
- purpose: declarations for Chebyshev algorithm implementation
- includes: "IMapView.hpp", "Position.hpp", <vector>
- defines class FOV with constructor explicit FOV(const core::IMapView &map);
- methods definitions:
    public:
      void compute(const Position &origin, int radius);
      bool isVisible(int x, int y) const;
    private:
      void castLight(int cx, int cy, int row, double startSlope, double endSlope,
                 int radius, int xx, int xy, int yx, int yy);
- state: functional, depends on IMapView, no longer includes world/Map.hpp

07.05. FOV.cpp
- purpose: Chebyshev algorithm
- includes: "FOV.hpp", <algorithm>, <cmath>
- methods implemented: FOV::FOV, FOV::compute, FOV::isVisible, FOV::castLight
- algorithm: Chebyshev
- map access through IMapView::blocksLos()
- state: stable and warning-free

07.06. Tile.hpp
- purpose: to describe a single tile on a map and define checking methods
- includes: <cstdint>
- declarations in namespace world
- class definition: enum class Tile, currently with Floor, Wall, DoorClosed, DoorOpen
- methods:
    blocksMove(Tile)
    blocksLos(Tile)
    isDoor(Tile)
    isWall(Tile)
    isFloor(Tile)
    isDoorClosed(Tile)
    isDoorOpen(Tile)

07.07. Map.hpp
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

07.08. Map.cpp
- purpose: map routines
- state: currently minimal

07.09. GenOptions.hpp
- purpose: contain shared CommonGenOptions base struct for all generators
- includes: none
- defines struct CommonGenOptions with field add_doors = true;

07.10. CavesGen.hpp
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

07.11. CavesGen.cpp
- purpose: Cellular Automata caves floor module generator implementation
- includes: "CavesGen.hpp", <algorithm>, <climits>, <queue>, <vector>
- helpers renamed to camelCase (resetToWalls -> fill(), countWallNeighbors8, caStep, etc.)
- uses declaration CompId = size_t to avoid static_casts
- implementation state: stable

07.12. RoomsGen.hpp
- purpose: definitions for rooms and corridors generator module
- includes: "GenOptions.hpp", "../Map.hpp", <random>
- declarations in namespace world
- declares:
    struct RoomsOptions : CommonGenOptions
        max_rooms = 35
        room_min = 5
        room_max = 10
    method generateRoomsModule(Map, RoomsOptions, std::mt19937)

07.13. RoomsGen.cpp
- purpose: Rooms and corridors floor module map generator
- includes: "RoomsGen.hpp", <algorithm>, <vector>
- helpers renamed to camelCase (resetToWalls -> fill(), areaIsAllWalls, carveRect, digToEdgeH/V, carveLCorridorStop)
- implementation state: working, corridors logic under refinement

07.14. MapGenerator.hpp
- purpose: declarations for the main floor generator engine
- includes: "../Map.hpp", <random>
- uses forward declarations of RoomsOptions and CavesOptions
- declarations in namespace world
- declares:
    class MapGenerator - generator engine class
    public:
      generateRooms(Map, RoomsOptions)
      generateCaves(Map, CavesOptions)
    private:
      placeDoors(Map)
      std::mt19937 &rng()
      std::mt19937 rng_;

07.15. MapGenerator.cpp
- purpose: main generator engine using modules
- includes: "MapGenerator.hpp", "CavesGen.hpp", "RoomsGen.hpp", <algorithm>
- implementations in namespace world
    MapGenerator::MapGenerator(uint)
    std::mt19937 &MapGenerator::rng()
    MapGenerator::generateRooms(Map, RoomsOptions)
    MapGenerator::generateCaves(Map, CavesOptions)
    MapGenerator::placeDoors(Map)
- uses m.fill(Tile::Wall) instead of local reset_to_walls()
- state: stable, integrated with refactored modules

#08. Compilation
- using CMake and CMakeLists.txt 

#09. Communication with the user
- brief and concise
- do not generate overblown responses unless asked to explain something in more detail
- do not generate many steps ahead
- if possible make a brief comment on design choices made

