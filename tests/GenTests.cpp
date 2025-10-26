#include "../src/world/Map.hpp"
#include "../src/world/Tile.hpp"
#include "../src/world/gen/CavesGen.hpp"
#include "../src/world/gen/MapGenerator.hpp"
#include "../src/world/gen/RoomsGen.hpp"
#include <cassert>
#include <iostream>

using namespace world;

int main() {
  // zamiast: using namespace world::gen;

  MapGenerator g(1337);

  // zamiast: Map m = g.generate(60, 40);
  Map m(60, 40, Tile::Wall);

  RoomsOptions ropt;
  ropt.max_rooms = 25;
  ropt.room_min = 4;
  ropt.room_max = 9;
  ropt.add_doors = true;

  g.generateRooms(m, ropt);

  // prosta asercja: na mapie mają być jakieś podłogi
  int floors = 0;
  for (int y = 0; y < m.height(); ++y)
    for (int x = 0; x < m.width(); ++x)
      if (isFloor(m.at({x, y})))
        ++floors;

  assert(floors > 0);
  std::cout << "GenTests (rooms) PASSED\n";

  // opcjonalnie drugi tryb:
  CavesOptions copt;
  copt.fill_percent = 45;
  copt.steps = 5;
  copt.add_doors = false;

  Map mc(60, 40, Tile::Wall);
  g.generateCaves(mc, copt);

  int cavesFloors = 0;
  for (int y = 0; y < mc.height(); ++y)
    for (int x = 0; x < mc.width(); ++x)
      if (isFloor(mc.at({x, y})))
        ++cavesFloors;

  assert(cavesFloors > 0);
  std::cout << "GenTests (caves) PASSED\n";

  return 0;
}
