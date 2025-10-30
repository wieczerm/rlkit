#include "FeatureManager.hpp"
#include "FeatureProperties.hpp"

namespace world {

void FeatureManager::addFeature(core::Position pos, Feature feature) {
  features_[pos] = std::move(feature);
}

void FeatureManager::removeFeature(core::Position pos) { features_.erase(pos); }

bool FeatureManager::hasFeature(core::Position pos) const {
  return features_.find(pos) != features_.end();
}

Feature *FeatureManager::getFeature(core::Position pos) {
  auto it = features_.find(pos);
  return it != features_.end() ? &it->second : nullptr;
}

const Feature *FeatureManager::getFeature(core::Position pos) const {
  auto it = features_.find(pos);
  return it != features_.end() ? &it->second : nullptr;
}

bool FeatureManager::blocksMovement(core::Position pos) const {
  const Feature *feature = getFeature(pos);
  return feature ? world::blocksMovement(*feature) : false;
}

bool FeatureManager::blocksLineOfSight(core::Position pos) const {
  const Feature *feature = getFeature(pos);
  return feature ? world::blocksLineOfSight(*feature) : false;
}

void FeatureManager::clear() { features_.clear(); }

std::vector<core::Position> FeatureManager::getAllPositions() const {
  std::vector<core::Position> positions;
  positions.reserve(features_.size());

  for (const auto &[pos, feature] : features_) {
    positions.push_back(pos);
  }

  return positions;
}

std::size_t FeatureManager::size() const { return features_.size(); }

} // namespace world
