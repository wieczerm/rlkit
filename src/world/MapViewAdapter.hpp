#pragma once
#include "Map.hpp"
#include "core/IMapView.hpp"
namespace world {
class MapViewAdapter final : public core::IMapView {
  const Map &m_;

public:
  explicit MapViewAdapter(const Map &m) : m_(m) {}
  int width() const noexcept override { return m_.width(); }
  int height() const noexcept override { return m_.height(); }
  bool blocksLineOfSight(int x, int y) const noexcept override {
    return m_.blocksLineOfSight({x, y});
  }
};
} // namespace world
