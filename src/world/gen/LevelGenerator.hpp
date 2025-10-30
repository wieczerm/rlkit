#pragma once
#include "FeaturePlacer.hpp"
#include "LevelData.hpp"
#include "config/DungeonConfig.hpp"
#include "world/FeatureManager.hpp"
#include "world/Map.hpp"
#include <memory>
#include <random>

namespace world {

// Main level generator - orchestrates terrain generation and feature placement
class LevelGenerator {
public:
  explicit LevelGenerator(std::mt19937 &rng);

  // Generate a complete level (map + features + spawn points)
  // Uses config to determine generator type and parameters
  LevelData generateLevel(int width, int height, int depth,
                          const config::LevelConfig &config);

  // Convenience overload using default config
  LevelData generateLevel(int width, int height, int depth);

private:
  // Generate terrain only (no features)
  void generateTerrain(Map &map, const config::LevelConfig &config);

  // Place features (doors, stairs) on terrain
  void placeFeatures(Map &map, FeatureManager &features, int target_depth,
                     const config::LevelConfig &config);

  // Find spawn points for player and monsters
  void findSpawnPoints(const Map &map, const FeatureManager &features,
                       LevelData &data, int monster_count);

  std::mt19937 &rng_;
  FeaturePlacer placer_;
};

} // namespace world
