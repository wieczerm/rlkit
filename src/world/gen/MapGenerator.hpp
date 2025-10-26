#pragma once
#include "../Map.hpp"
#include <random>

namespace world {
struct RoomsOptions; // forward
struct CavesOptions; // forward

class MapGenerator {
public:
  explicit MapGenerator(uint32_t seed);
  void generateRooms(Map &m, const RoomsOptions &opt);
  void generateCaves(Map &m, const CavesOptions &opt);

private:
  void resetToWalls(Map &m);
  void placeDoors(Map &m);
  std::mt19937 &rng();
  std::mt19937 rng_;
};
} // namespace world
