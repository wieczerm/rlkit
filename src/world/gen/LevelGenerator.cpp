#include "CavesGen.hpp"
#include "LevelGenerator.hpp"
#include "RoomsGen.hpp"
#include "config/DungeonConfig.hpp"
#include "world/Tile.hpp"

namespace world {

LevelGenerator::LevelGenerator(std::mt19937 &rng) : rng_(rng), placer_(rng) {}

LevelData LevelGenerator::generateLevel(int width, int height, int depth,
                                        const config::LevelConfig &config) {
  // Create map and feature manager
  auto map = std::make_unique<Map>(width, height, Tile::SolidRock);
  auto features = std::make_unique<FeatureManager>();

  // Step 1: Generate terrain (only Floor and Wall)
  generateTerrain(*map, config);

  // Step 2: Place features (doors and stairs)
  int target_depth = depth + 1; // Stairs lead to next level
  placeFeatures(*map, *features, target_depth, config);

  // Step 3: Create LevelData and find spawn points
  LevelData data(std::move(map), std::move(features));
  data.depth = depth;

  // Find spawn points (20 monsters for now - could be config)
  findSpawnPoints(*data.map, *data.features, data, 20);

  return data;
}

LevelData LevelGenerator::generateLevel(int width, int height, int depth) {
  // Use default config for this depth
  // config::LevelConfig config = config::getConfigForDepth(depth);
  config::LevelConfig config =
      config::largeDungeon(); // mw: hardcoded largeDungeon as fallback
  return generateLevel(width, height, depth, config);
}

void LevelGenerator::generateTerrain(Map &map,
                                     const config::LevelConfig &config) {
  // Choose generator based on config
  // mw: comented out all swtiches to leave with default only - testing
  //  switch (config.generator_type) {
  // case config::GeneratorType::Rooms:
  //   generateRoomsModule(map, config, rng_);
  //   break;

  // case config::GeneratorType::Caves:
  //  generateCavesModule(map, config, rng_);
  //  break;

  // default:
  // Fallback to rooms if unknown type
  generateRoomsModule(map, config, rng_);
  //   break;
  // }
}

void LevelGenerator::placeFeatures(Map &map, FeatureManager &features,
                                   int target_depth,
                                   const config::LevelConfig &config) {
  // Place doors if enabled
  if (config.rooms.add_doors) {
    placer_.placeDoors(map, features);
  }

  // Place stairs
  // TODO: Make stairs parameters configurable via config
  placer_.placeStairs(map, features, target_depth, 1, 3, 0.15, 0.05);
}

void LevelGenerator::findSpawnPoints(const Map &map,
                                     const FeatureManager &features,
                                     LevelData &data, int monster_count) {
  // Find player spawn (single position, away from stairs)
  auto player_spawns = placer_.findValidSpawns(map, features, 1, 5);
  if (!player_spawns.empty()) {
    data.player_spawn = player_spawns[0];
  } else {
    // Fallback: find any floor tile
    for (int y = 1; y < map.height() - 1; ++y) {
      for (int x = 1; x < map.width() - 1; ++x) {
        if (!map.blocksMovement({x, y})) {
          data.player_spawn = {x, y};
          goto player_found;
        }
      }
    }
  player_found:;
  }

  // Find monster spawns (away from stairs)
  data.monster_spawns =
      placer_.findValidSpawns(map, features, monster_count, 5);
}

} // namespace world
