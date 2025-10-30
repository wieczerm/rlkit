#pragma once
#include "core/Position.hpp"
#include <cstdint>
#include <variant>

namespace world {

// Door feature - can be opened/closed
// Future: add Locked state when inventory system exists
struct Door {
  enum class Material : std::uint8_t { Wood, Iron, Stone };

  enum class State : std::uint8_t {
    Open,
    Closed
    // Future: Locked (requires key from inventory)
  };

  Material material;
  State state;

  // Future: key_id for locked doors
  // int key_id;  // -1 if no key, or item ID required
};

// Stairs feature - transition between levels
struct Stairs {
  enum class Direction : std::uint8_t { Down, Up };

  Direction direction;
  int target_depth; // Which level this leads to
};

// Feature variant - add new feature types here
using Feature = std::variant<Door, Stairs>;

} // namespace world
