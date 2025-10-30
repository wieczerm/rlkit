#pragma once
#include "core/Position.hpp"
#include "world/FeatureManager.hpp"
#include "world/Map.hpp"
#include <random>
#include <vector>

namespace world {

// Places features (doors, stairs) on generated terrain
class FeaturePlacer {
public:
  explicit FeaturePlacer(std::mt19937 &rng);

  // Place doors at corridor-room junctions
  void placeDoors(Map &map, FeatureManager &features);

  // Place stairs in valid floor positions
  // Returns positions where stairs were placed
  std::vector<core::Position>
  placeStairs(Map &map, FeatureManager &features, int target_depth,
              int min_stairs = 1, int max_stairs = 3,
              double second_stairs_chance = 0.15,
              double third_stairs_chance = 0.05);

  // Find valid spawn positions (floor, not near stairs, not blocked)
  std::vector<core::Position>
  findValidSpawns(const Map &map, const FeatureManager &features, int count,
                  int min_distance_from_stairs = 5);

private:
  // Check if position is valid for door placement
  bool isValidDoorPosition(const Map &map, core::Position pos) const;

  // Check if position is valid for stairs placement
  bool isValidStairsPosition(const Map &map, const FeatureManager &features,
                             core::Position pos) const;

  // Find all candidate positions for stairs
  std::vector<core::Position>
  findStairsCandidates(const Map &map, const FeatureManager &features) const;

  std::mt19937 &rng_;
};

} // namespace world
