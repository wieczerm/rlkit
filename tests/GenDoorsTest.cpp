// tests/GenDoorTest.cpp
#include "../src/world/Map.hpp"
#include "../src/world/Tile.hpp"
#include "../src/world/gen/CavesGen.hpp"
#include "../src/world/gen/RoomsGen.hpp"

#include "../src/world/gen/MapGenerator.hpp"
#include <cassert>
#include <iostream>

using namespace world;

static int countTiles(const Map &m, Tile t) {
  int c = 0;
  for (int y = 0; y < m.height(); ++y)
    for (int x = 0; x < m.width(); ++x)
      if (m.at({x, y}) == t)
        ++c;
  return c;
}

int main() {
  constexpr int W = 60, H = 30;
  Map m(W, H, Tile::Wall);

  MapGenerator gen(1337);
  RoomsOptions opt;
  opt.max_rooms = 25;
  opt.room_min = 4;
  opt.room_max = 9;
  opt.add_doors = true;

  gen.generateRooms(m, opt);

  int doorCount = countTiles(m, Tile::DoorClosed);
  std::cout << "Drzwi: " << doorCount << "\n";
  assert(doorCount > 0 && "Brak wygenerowanych drzwi");

  for (int y = 1; y < H - 1; ++y) {
    for (int x = 1; x < W - 1; ++x) {
      if (m.at({x, y}) == Tile::DoorClosed) {
        bool floorL = isFloor(m.at({x - 1, y}));
        bool floorR = isFloor(m.at({x + 1, y}));
        bool floorU = isFloor(m.at({x, y - 1}));
        bool floorD = isFloor(m.at({x, y + 1}));

        bool horiz = floorL && floorR && !floorU && !floorD;
        bool vert = floorU && floorD && !floorL && !floorR;

        assert((horiz || vert) && "Drzwi w niepoprawnym miejscu");
      }
    }
  }

  std::cout << "Test GenDoorTest PASSED\n";
  return 0;
}
