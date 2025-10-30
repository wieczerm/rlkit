#pragma once
#include "core/Serialization.hpp"
#include "entities/EntityManager.hpp"
#include "world/FeatureManager.hpp"
#include "world/Map.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace serialization {

// State of a single level (for level stacking)
struct LevelState {
  world::Map map;
  world::FeatureManager features;
  entities::EntityManager entities;
  std::vector<bool> discovered; // Discovered tiles (parallel to map)
  int depth;

  // No default constructor - Map requires dimensions
  LevelState(world::Map m, world::FeatureManager f, entities::EntityManager e,
             std::vector<bool> d, int dep)
      : map(std::move(m)), features(std::move(f)), entities(std::move(e)),
        discovered(std::move(d)), depth(dep) {}
};

// Complete save game data
struct SaveData {
  std::vector<LevelState> visited_levels;
  int current_level_index; // Which level player is currently on
  int turn_counter;        // Total game turns

  SaveData() : current_level_index(0), turn_counter(0) {}
};

// Save game to file
void saveGame(const SaveData &data, const std::string &filename);

// Load game from file (throws on error)
SaveData loadGame(const std::string &filename);

} // namespace serialization

// JSON serialization for game structures - must be in serialization namespace
// for ADL
namespace serialization {

void to_json(nlohmann::json &j, const LevelState &ls);
void from_json(const nlohmann::json &j, LevelState &ls);

void to_json(nlohmann::json &j, const SaveData &sd);
void from_json(const nlohmann::json &j, SaveData &sd);

} // namespace serialization
