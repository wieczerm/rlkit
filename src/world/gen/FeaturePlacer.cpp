#include "FeaturePlacer.hpp"
#include "world/Feature.hpp"
#include "world/FeatureProperties.hpp"
#include "world/TileProperties.hpp"
#include <algorithm>
#include <cmath>

namespace world {

FeaturePlacer::FeaturePlacer(std::mt19937 &rng) : rng_(rng) {}

void FeaturePlacer::placeDoors(Map &map, FeatureManager &features) {
  // Scan for valid door positions (wall between two floor tiles)
  for (int y = 1; y < map.height() - 1; ++y) {
    for (int x = 1; x < map.width() - 1; ++x) {
      core::Position pos{x, y};

      if (!blocksMovement(map.at(pos))) {
        continue; // Not a wall
      }

      if (isValidDoorPosition(map, pos)) {
        // Place closed door
        features.addFeature(pos,
                            Door{Door::Material::Wood, Door::State::Closed});
      }
    }
  }
}

bool FeaturePlacer::isValidDoorPosition(const Map &map,
                                        core::Position pos) const {
  // Must be on a wall
  if (!blocksMovement(map.at(pos))) {
    return false;
  }

  // Check neighbors
  const bool floorL = !blocksMovement(map.at({pos.x - 1, pos.y}));
  const bool floorR = !blocksMovement(map.at({pos.x + 1, pos.y}));
  const bool floorU = !blocksMovement(map.at({pos.x, pos.y - 1}));
  const bool floorD = !blocksMovement(map.at({pos.x, pos.y + 1}));

  // Horizontal door: floor left and right, walls up and down
  const bool horiz = floorL && floorR && !floorU && !floorD;

  // Vertical door: floor up and down, walls left and right
  const bool vert = floorU && floorD && !floorL && !floorR;

  return horiz || vert;
}

std::vector<core::Position>
FeaturePlacer::placeStairs(Map &map, FeatureManager &features, int target_depth,
                           int min_stairs, int max_stairs,
                           double second_stairs_chance,
                           double third_stairs_chance) {
  std::vector<core::Position> placed_stairs;

  // Find all valid candidates
  auto candidates = findStairsCandidates(map, features);

  if (candidates.empty()) {
    return placed_stairs; // No valid positions
  }

  // Shuffle candidates for randomness
  std::shuffle(candidates.begin(), candidates.end(), rng_);

  std::uniform_real_distribution<double> chance(0.0, 1.0);

  // Place first stairs (guaranteed if we have at least min_stairs candidates)
  if (!candidates.empty() &&
      placed_stairs.size() < static_cast<size_t>(max_stairs)) {
    core::Position pos = candidates.back();
    candidates.pop_back();

    features.addFeature(pos, Stairs{Stairs::Direction::Down, target_depth});
    placed_stairs.push_back(pos);
  }

  // Place second stairs (probability-based)
  if (!candidates.empty() &&
      placed_stairs.size() < static_cast<size_t>(max_stairs) &&
      chance(rng_) < second_stairs_chance) {

    core::Position pos = candidates.back();
    candidates.pop_back();

    features.addFeature(pos, Stairs{Stairs::Direction::Down, target_depth});
    placed_stairs.push_back(pos);
  }

  // Place third stairs (lower probability)
  if (!candidates.empty() &&
      placed_stairs.size() < static_cast<size_t>(max_stairs) &&
      chance(rng_) < third_stairs_chance) {

    core::Position pos = candidates.back();
    candidates.pop_back();

    features.addFeature(pos, Stairs{Stairs::Direction::Down, target_depth});
    placed_stairs.push_back(pos);
  }

  return placed_stairs;
}

std::vector<core::Position> FeaturePlacer::findStairsCandidates(
    const Map &map, const FeatureManager &features) const {
  std::vector<core::Position> candidates;

  // Scan entire map for valid positions
  for (int y = 2; y < map.height() - 2; ++y) {
    for (int x = 2; x < map.width() - 2; ++x) {
      core::Position pos{x, y};

      if (isValidStairsPosition(map, features, pos)) {
        candidates.push_back(pos);
      }
    }
  }

  return candidates;
}

bool FeaturePlacer::isValidStairsPosition(const Map &map,
                                          const FeatureManager &features,
                                          core::Position pos) const {
  // Must be on floor
  if (blocksMovement(map.at(pos))) {
    return false;
  }

  // Must not already have a feature
  if (features.hasFeature(pos)) {
    return false;
  }

  // Must have at least 2 walkable neighbors (not isolated)
  int walkable_neighbors = 0;
  for (int dy = -1; dy <= 1; ++dy) {
    for (int dx = -1; dx <= 1; ++dx) {
      if (dx == 0 && dy == 0)
        continue;

      core::Position neighbor{pos.x + dx, pos.y + dy};
      if (map.inBounds(neighbor) && !blocksMovement(map.at(neighbor))) {
        walkable_neighbors++;
      }
    }
  }

  if (walkable_neighbors < 2) {
    return false; // Too isolated
  }

  // Prefer positions not directly adjacent to walls on all sides (avoid
  // corridors) This is a soft preference - we still allow corridor stairs if
  // needed
  int wall_neighbors = 0;
  for (int dy = -1; dy <= 1; ++dy) {
    for (int dx = -1; dx <= 1; ++dx) {
      if (dx == 0 && dy == 0)
        continue;

      core::Position neighbor{pos.x + dx, pos.y + dy};
      if (map.inBounds(neighbor) && blocksMovement(map.at(neighbor))) {
        wall_neighbors++;
      }
    }
  }

  // Prefer open areas (fewer walls nearby)
  // But don't reject corridor positions entirely
  return wall_neighbors <= 6; // Allow some walls
}

std::vector<core::Position> FeaturePlacer::findValidSpawns(
    const Map &map, const FeatureManager &features, int count,
    int min_distance_from_stairs) {
  std::vector<core::Position> spawns;
  std::vector<core::Position> all_candidates;

  // Find all stairs positions
  std::vector<core::Position> stairs_positions;
  for (const auto &pos : features.getAllPositions()) {
    const Feature *f = features.getFeature(pos);
    if (f && isStairs(*f)) {
      stairs_positions.push_back(pos);
    }
  }

  // Find all valid spawn candidates
  for (int y = 1; y < map.height() - 1; ++y) {
    for (int x = 1; x < map.width() - 1; ++x) {
      core::Position pos{x, y};

      // Must be floor
      if (blocksMovement(map.at(pos))) {
        continue;
      }

      // Must not have a feature
      if (features.hasFeature(pos)) {
        continue;
      }

      // Must not be too close to stairs
      bool too_close = false;
      for (const auto &stairs_pos : stairs_positions) {
        int dist = std::abs(pos.x - stairs_pos.x) + std::abs(pos.y - stairs_pos.y);
        if (dist < min_distance_from_stairs) {
          too_close = true;
          break;
        }
      }

      if (!too_close) {
        all_candidates.push_back(pos);
      }
    }
  }

  // Shuffle and take requested count
  std::shuffle(all_candidates.begin(), all_candidates.end(), rng_);

  int to_take = std::min(count, static_cast<int>(all_candidates.size()));
  spawns.insert(spawns.end(), all_candidates.begin(),
                all_candidates.begin() + to_take);

  return spawns;
}

} // namespace world
