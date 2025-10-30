#include "TileRegistry.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace world {

TileRegistry::TileRegistry() { initDefaults(); }

TileRegistry &TileRegistry::instance() {
  static TileRegistry instance;
  return instance;
}

void TileRegistry::initDefaults() {
  // Minimal safe defaults for all defined tile types

  // OpenGround: walkable, visible, no damage
  registerTile(Tile::OpenGround,
               TileProperties{
                   100,   // movement_cost: normal speed
                   false, // blocks_los: does not block vision
                   0,     // damage_immediate: no damage on entry
                   0      // damage_per_turn: no damage while standing
               });

  // SolidRock: impassable, blocks vision, no damage
  registerTile(Tile::SolidRock, TileProperties{
                                    -1,   // movement_cost: impassable
                                    true, // blocks_los: blocks vision
                                    0,    // damage_immediate: no damage
                                    0     // damage_per_turn: no damage
                                });

  // ShallowLiquid: slow movement, visible, no damage (predefined)
  registerTile(Tile::ShallowLiquid,
               TileProperties{
                   200,   // movement_cost: half speed
                   false, // blocks_los: does not block vision
                   0,     // damage_immediate: no damage
                   0      // damage_per_turn: no damage
               });

  // DeepLiquid: impassable, visible, no damage (predefined)
  registerTile(Tile::DeepLiquid,
               TileProperties{
                   -1,    // movement_cost: impassable (need swimming)
                   false, // blocks_los: does not block vision
                   0,     // damage_immediate: no damage
                   0      // damage_per_turn: no damage
               });
}

void TileRegistry::registerTile(Tile t, const TileProperties &props) {
  properties_[t] = props;
}

const TileProperties &TileRegistry::getProperties(Tile t) const {
  auto it = properties_.find(t);
  if (it == properties_.end()) {
    throw std::runtime_error(
        "TileRegistry: No properties registered for tile type");
  }
  return it->second;
}

bool TileRegistry::hasProperties(Tile t) const {
  return properties_.find(t) != properties_.end();
}

void TileRegistry::loadFromJSON(const std::string &path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    std::cerr << "Warning: Could not open tile config file: " << path
              << "\nUsing hardcoded defaults." << std::endl;
    return;
  }

  try {
    nlohmann::json j;
    file >> j;

    // Expect format: { "tiles": { "OpenGround": { ... }, ... } }
    if (!j.contains("tiles")) {
      std::cerr << "Warning: JSON missing 'tiles' key in: " << path
                << "\nUsing hardcoded defaults." << std::endl;
      return;
    }

    const auto &tiles = j["tiles"];
    for (const auto &[key, value] : tiles.items()) {
      // Convert string key to Tile enum
      auto tile_opt = tileFromString(key);
      if (!tile_opt) {
        std::cerr << "Warning: Unknown tile type in JSON: " << key
                  << " (skipping)" << std::endl;
        continue;
      }

      // Parse properties
      TileProperties props;
      props.movement_cost = value.value("movement_cost", -1);
      props.blocks_los = value.value("blocks_los", true);
      props.damage_immediate = value.value("damage_immediate", 0);
      props.damage_per_turn = value.value("damage_per_turn", 0);

      // Register (overwrites defaults)
      registerTile(*tile_opt, props);
    }

    std::cout << "Loaded tile properties from: " << path << std::endl;

  } catch (const nlohmann::json::exception &e) {
    std::cerr << "Warning: JSON parse error in " << path << ": " << e.what()
              << "\nUsing hardcoded defaults." << std::endl;
  }
}

// Implementation of wrapper function
const TileProperties &getProperties(Tile t) {
  return TileRegistry::instance().getProperties(t);
}

} // namespace world
