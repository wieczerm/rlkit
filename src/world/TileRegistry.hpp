#pragma once
#include "Tile.hpp"
#include "TileEnum.hpp"
#include "TileProperties.hpp"
#include <map>
#include <string>

namespace world {

// Singleton registry managing tile properties
// Loads from JSON config with hardcoded fallback defaults
class TileRegistry {
public:
  // Singleton access
  static TileRegistry &instance();

  // Load tile properties from JSON file
  // Falls back to hardcoded defaults if load fails
  void loadFromJSON(const std::string &path);

  // Register a tile type with its properties
  void registerTile(Tile t, const TileProperties &props);

  // Query properties for a tile type
  // Returns reference to avoid copies
  const TileProperties &getProperties(Tile t) const;

  // Check if a tile type is registered
  bool hasProperties(Tile t) const;

  // Delete copy/move constructors (singleton)
  TileRegistry(const TileRegistry &) = delete;
  TileRegistry &operator=(const TileRegistry &) = delete;
  TileRegistry(TileRegistry &&) = delete;
  TileRegistry &operator=(TileRegistry &&) = delete;

private:
  // Private constructor - use instance() instead
  TileRegistry();

  // Initialize hardcoded fallback defaults
  void initDefaults();

  // Storage: ordered map (O(log n) lookup, acceptable for small tile count)
  std::map<Tile, TileProperties> properties_;
};

} // namespace world
