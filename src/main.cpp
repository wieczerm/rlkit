// src/main.cpp
#include "core/FOV.hpp"
#include "world/Map.hpp"
#include "world/MapViewAdapter.hpp"
#include "world/Tile.hpp"
#include "world/gen/CavesGen.hpp"
#include "world/gen/MapGenerator.hpp"
#include "world/gen/RoomsGen.hpp"
#include <cstdlib>
#include <iostream>
#include <string>

using namespace world;

static char glyph(Tile t) {
  if (isWall(t))
    return '#';
  if (isDoorClosed(t))
    return '+';
  if (isDoorOpen(t))
    return '/';
  // Floor (w tym płytka drzwi jako przejście)
  return '.';
}

static void printMap(const Map &m) {
  for (int y = 0; y < m.height(); ++y) {
    for (int x = 0; x < m.width(); ++x) {
      std::cout << glyph(m.at({x, y}));
    }
    std::cout << '\n';
  }
}

int main(int argc, char **argv) {
  // Args
  std::string mode = (argc > 1) ? argv[1] : "rooms";
  int W = (argc > 2) ? std::atoi(argv[2]) : 60;
  int H = (argc > 3) ? std::atoi(argv[3]) : 30;
  unsigned seed =
      (argc > 4) ? static_cast<unsigned>(std::strtoul(argv[4], nullptr, 10))
                 : 1337u;

  if (W < 4)
    W = 4;
  if (H < 4)
    H = 4;

  // Map
  Map m(W, H, Tile::Wall);

  // Generator
  MapGenerator gen(seed);

  if (mode == "rooms") {
    RoomsOptions opt;
    opt.max_rooms = 35;
    opt.room_min = 5;
    opt.room_max = 10;
    opt.add_doors = true;
    gen.generateRooms(m, opt);
  } else if (mode == "caves") {
    CavesOptions opt;
    opt.fill_percent = 52;
    opt.steps = 5;
    opt.add_doors = false; // zwykle OFF dla jaskiń
    gen.generateCaves(m, opt);
  } else {
    std::cerr << "Tryb nieznany. Uzycie: " << argv[0]
              << " [rooms|caves] [W] [H] [seed]\n";
    return 1;
  }

  printMap(m);
  return 0;
}
