#include "MapGenerator.hpp"
#include "CavesGen.hpp"
#include "RoomsGen.hpp"
#include <algorithm>

using namespace world;

namespace world {

MapGenerator::MapGenerator(unsigned int seed) {
  if (seed == 0) {
    std::random_device rd;
    rng_.seed(rd());
  } else {
    rng_.seed(seed);
  }
}

std::mt19937 &MapGenerator::rng() { return rng_; }

void MapGenerator::generateRooms(Map &m, const RoomsOptions &opt) {
  generateRoomsModule(m, opt, rng_);
  if (opt.add_doors)
    placeDoors(m);
}

void MapGenerator::generateCaves(Map &m, const CavesOptions &opt) {
  generateCavesModule(m, opt, rng_);
  if (opt.add_doors)
    placeDoors(m); // usually keep false for caves
}

void MapGenerator::placeDoors(Map &m) {
  for (int y = 1; y < m.height() - 1; ++y) {
    for (int x = 1; x < m.width() - 1; ++x) {
      if (!isWall(m.at({x, y})))
        continue;

      const bool floorL = isFloor(m.at({x - 1, y}));
      const bool floorR = isFloor(m.at({x + 1, y}));
      const bool floorU = isFloor(m.at({x, y - 1}));
      const bool floorD = isFloor(m.at({x, y + 1}));

      const bool horiz = floorL && floorR && !floorU && !floorD;
      const bool vert = floorU && floorD && !floorL && !floorR;

      if (horiz || vert)
        m.set({x, y}, Tile::DoorClosed);
    }
  }
}

} // namespace world
