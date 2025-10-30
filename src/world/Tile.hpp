#pragma once
#include <cstdint>

namespace world {

// Material-based tile types representing terrain layer
enum class Tile : std::uint8_t {
  OpenGround,    // walkable terrain: floors, grass, dirt, paths
  SolidRock,     // impassable solid: walls, mountains, dense stone
  ShallowLiquid, // difficult terrain: shallow water, mud (predefined, not
                 // implemented)
  DeepLiquid,    // impassable liquid: deep water, deep lava (predefined, not
                 // implemented)

  // Future expansion (placeholder for forward compatibility):
  // Ice,        // slippery terrain with modified movement
  // Lava,       // damaging terrain with fire damage
  // Chasm,      // impassable void/pit
  // Bridge,     // special walkable over liquid
  // Sand,       // difficult terrain with movement penalty
};

// Number of defined tile types (for iteration/validation)
constexpr std::size_t TILE_COUNT = 4;

} // namespace world
