#pragma once
#include "Feature.hpp"
#include "core/Position.hpp"
#include <functional>
#include <unordered_map>

namespace core {
// Forward declare Position hash for unordered_map
struct PositionHash {
  std::size_t operator()(const Position &p) const noexcept {
    return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
  }
};
} // namespace core

namespace world {

// Manages features on the map
// One feature per tile maximum
// Features are stored by position for O(1) lookup
class FeatureManager {
public:
  FeatureManager() = default;

  // Add feature at position (replaces existing if any)
  void addFeature(core::Position pos, Feature feature);

  // Remove feature at position (no-op if none exists)
  void removeFeature(core::Position pos);

  // Query
  bool hasFeature(core::Position pos) const;

  // Get feature at position (returns nullptr if none)
  Feature *getFeature(core::Position pos);
  const Feature *getFeature(core::Position pos) const;

  // Property queries (convenience wrappers)
  bool blocksMovement(core::Position pos) const;
  bool blocksLineOfSight(core::Position pos) const;

  // Clear all features
  void clear();

  // Get all feature positions (for iteration)
  std::vector<core::Position> getAllPositions() const;

  // Count features
  std::size_t size() const;

private:
  std::unordered_map<core::Position, Feature, core::PositionHash> features_;
};

} // namespace world
