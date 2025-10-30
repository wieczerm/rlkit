#include "serialization/GameSerialization.hpp"
#include <fstream>
#include <stdexcept>

using json = nlohmann::json;

namespace serialization {

void saveGame(const SaveData &data, const std::string &filename) {
  json j;
  to_json(j, data); // Explicit call

  std::ofstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file for writing: " + filename);
  }

  file << j.dump(2); // Pretty print with 2-space indent

  if (!file.good()) {
    throw std::runtime_error("Error writing to file: " + filename);
  }
}

SaveData loadGame(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file for reading: " + filename);
  }

  json j;
  try {
    file >> j;
  } catch (const json::exception &e) {
    throw std::runtime_error("JSON parse error: " + std::string(e.what()));
  }

  SaveData data;
  from_json(j, data); // Explicit call
  return data;
}

} // namespace serialization

// Serialization implementations for world types - in world namespace
namespace world {

// Map serialization
void to_json(json &j, const Map &m) {
  j = json{
      {"width", m.width()}, {"height", m.height()}, {"tiles", json::array()}};

  // Serialize tiles as array of enum values
  for (int y = 0; y < m.height(); ++y) {
    for (int x = 0; x < m.width(); ++x) {
      j["tiles"].push_back(m.at({x, y}));
    }
  }
}

void from_json(const json &j, Map &m) {
  int width = j.at("width").get<int>();
  int height = j.at("height").get<int>();

  // Create map with default fill (will be overwritten)
  m = Map(width, height, Tile::OpenGround);

  // Restore tiles
  const auto &tiles = j.at("tiles");
  int idx = 0;
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      Tile tile = tiles[idx++].get<Tile>();
      m.set({x, y}, tile);
    }
  }
}

// FeatureManager serialization
void to_json(json &j, const FeatureManager &fm) {
  j = json::array();

  // Get all feature positions
  auto positions = fm.getAllPositions();

  for (const auto &pos : positions) {
    const world::Feature *feature = fm.getFeature(pos);
    if (feature) {
      j.push_back(json{{"position", pos}, {"feature", *feature}});
    }
  }
}

void from_json(const json &j, FeatureManager &fm) {
  fm.clear();

  for (const auto &item : j) {
    core::Position pos = item.at("position").get<core::Position>();
    Feature feature = item.at("feature").get<Feature>();
    fm.addFeature(pos, std::move(feature));
  }
}

} // namespace world

// EntityManager serialization - in entities namespace
namespace entities {

void to_json(json &j, const EntityManager &em) {
  j = json::array();

  // Serialize all entities
  const auto &entities = em.getEntities(); // Changed from getAllEntities
  for (const auto &entity_ptr : entities) {
    if (entity_ptr) {
      j.push_back(*entity_ptr);
    }
  }
}

void from_json(const json &j, EntityManager &em) {
  em.clear();

  for (const auto &entity_json : j) {
    auto entity = std::make_unique<Entity>("", core::Position{0, 0});
    entity_json.get_to(*entity);
    em.addEntity(std::move(entity));
  }
}

} // namespace entities

// Game state serialization - in serialization namespace
namespace serialization {

void to_json(json &j, const LevelState &ls) {
  j = json{{"depth", ls.depth},
           {"map", ls.map},
           {"features", ls.features},
           {"entities", ls.entities},
           {"discovered", ls.discovered}};
}

void from_json(const json &j, LevelState &ls) {
  j.at("depth").get_to(ls.depth);
  j.at("map").get_to(ls.map);
  j.at("features").get_to(ls.features);
  j.at("entities").get_to(ls.entities);
  j.at("discovered").get_to(ls.discovered);
}

// SaveData serialization
void to_json(json &j, const SaveData &sd) {
  j = json{{"turn_counter", sd.turn_counter},
           {"current_level_index", sd.current_level_index},
           {"visited_levels", sd.visited_levels}};
}

void from_json(const json &j, SaveData &sd) {
  j.at("turn_counter").get_to(sd.turn_counter);
  j.at("current_level_index").get_to(sd.current_level_index);

  // Manually convert visited_levels array
  const auto &levels_json = j.at("visited_levels");
  sd.visited_levels.clear();
  sd.visited_levels.reserve(levels_json.size());

  for (const auto &level_json : levels_json) {
    // Deserialize components first
    world::Map map(1, 1); // Temporary, will be replaced
    world::FeatureManager features;
    entities::EntityManager entities_mgr;
    std::vector<bool> discovered;
    int depth = 0;

    level_json.at("depth").get_to(depth);
    level_json.at("map").get_to(map);
    level_json.at("features").get_to(features);
    level_json.at("entities").get_to(entities_mgr);
    level_json.at("discovered").get_to(discovered);

    // Construct LevelState with all components
    sd.visited_levels.emplace_back(std::move(map), std::move(features),
                                   std::move(entities_mgr),
                                   std::move(discovered), depth);
  }
}

} // namespace serialization
