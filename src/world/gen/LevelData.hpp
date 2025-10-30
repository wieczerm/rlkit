#pragma once
#include "core/Position.hpp"
#include "world/FeatureManager.hpp"
#include "world/Map.hpp"
#include <memory>
#include <vector>

namespace world {

// Complete level data returned by LevelGenerator
struct LevelData {
  std::unique_ptr<Map> map;
  std::unique_ptr<FeatureManager> features;

  core::Position player_spawn;
  std::vector<core::Position> monster_spawns;

  // Optional metadata
  int depth;
  int room_count;

  LevelData()
      : map(nullptr), features(nullptr), player_spawn{0, 0}, depth(0),
        room_count(0) {}

  LevelData(std::unique_ptr<Map> m, std::unique_ptr<FeatureManager> f)
      : map(std::move(m)), features(std::move(f)), player_spawn{0, 0}, depth(0),
        room_count(0) {}
};

} // namespace world
