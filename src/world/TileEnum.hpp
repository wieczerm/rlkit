#pragma once
#include "Tile.hpp" // Must include full enum definition, not forward declare
#include <optional>
#include <string>

namespace world {

// Convert Tile enum to string representation
inline const char *tileToString(Tile t) noexcept {
  switch (t) {
  case Tile::OpenGround:
    return "OpenGround";
  case Tile::SolidRock:
    return "SolidRock";
  case Tile::ShallowLiquid:
    return "ShallowLiquid";
  case Tile::DeepLiquid:
    return "DeepLiquid";
  }
  return "Unknown";
}

// Convert string to Tile enum (returns nullopt if invalid)
inline std::optional<Tile> tileFromString(const std::string &str) noexcept {
  if (str == "OpenGround")
    return Tile::OpenGround;
  if (str == "SolidRock")
    return Tile::SolidRock;
  if (str == "ShallowLiquid")
    return Tile::ShallowLiquid;
  if (str == "DeepLiquid")
    return Tile::DeepLiquid;

  return std::nullopt; // Invalid string
}

// Macro for easy addition of new tile types
// Usage: Add case to both functions above when adding new Tile enum value
#define TILE_STRING_CASE(tile_name)                                            \
  case Tile::tile_name:                                                        \
    return #tile_name;

#define TILE_FROM_STRING_CASE(tile_name)                                       \
  if (str == #tile_name)                                                       \
    return Tile::tile_name;

} // namespace world
