#pragma once
#include "Feature.hpp"

namespace world {

// Query feature properties dynamically based on feature state
// Example: Closed door blocks movement, open door does not

// Does feature block movement at current state?
inline bool blocksMovement(const Feature &f) {
  return std::visit(
      [](const auto &feature) -> bool {
        using T = std::decay_t<decltype(feature)>;

        if constexpr (std::is_same_v<T, Door>) {
          return feature.state == Door::State::Closed;
        } else if constexpr (std::is_same_v<T, Stairs>) {
          return false; // Stairs never block movement
        }

        return false;
      },
      f);
}

// Does feature block line of sight at current state?
inline bool blocksLineOfSight(const Feature &f) {
  return std::visit(
      [](const auto &feature) -> bool {
        using T = std::decay_t<decltype(feature)>;

        if constexpr (std::is_same_v<T, Door>) {
          return feature.state == Door::State::Closed;
        } else if constexpr (std::is_same_v<T, Stairs>) {
          return false; // Stairs never block LOS
        }

        return false;
      },
      f);
}

// Can feature be interacted with?
inline bool isInteractable(const Feature &f) {
  return std::visit(
      [](const auto &feature) -> bool {
        using T = std::decay_t<decltype(feature)>;

        if constexpr (std::is_same_v<T, Door>) {
          return true; // Doors can always be opened/closed
        } else if constexpr (std::is_same_v<T, Stairs>) {
          return true; // Stairs can be used (descend/ascend)
        }

        return false;
      },
      f);
}

// Get glyph for rendering
inline char getGlyph(const Feature &f) {
  return std::visit(
      [](const auto &feature) -> char {
        using T = std::decay_t<decltype(feature)>;

        if constexpr (std::is_same_v<T, Door>) {
          return feature.state == Door::State::Open ? '\'' : '+';
        } else if constexpr (std::is_same_v<T, Stairs>) {
          return feature.direction == Stairs::Direction::Down ? '>' : '<';
        }

        return '?';
      },
      f);
}

// Check if feature is a door
inline bool isDoor(const Feature &f) { return std::holds_alternative<Door>(f); }

// Check if feature is stairs
inline bool isStairs(const Feature &f) {
  return std::holds_alternative<Stairs>(f);
}

// Get door from feature (returns nullptr if not a door)
inline Door *getDoor(Feature &f) { return std::get_if<Door>(&f); }

inline const Door *getDoor(const Feature &f) { return std::get_if<Door>(&f); }

// Get stairs from feature (returns nullptr if not stairs)
inline Stairs *getStairs(Feature &f) { return std::get_if<Stairs>(&f); }

inline const Stairs *getStairs(const Feature &f) {
  return std::get_if<Stairs>(&f);
}

} // namespace world
