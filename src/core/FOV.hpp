#pragma once
#include "IMapView.hpp"
#include "Position.hpp"
#include <vector>

namespace core {

class FOV {
public:
  explicit FOV(const core::IMapView &map);

  void compute(const Position &origin, int radius);
  bool isVisible(int x, int y) const;

private:
  const core::IMapView &map_;
  size_t width_;
  size_t height_;
  std::vector<std::vector<bool>> visible_;

  // Simple Bresenham line-of-sight check
  bool hasLineOfSight(int x0, int y0, int x1, int y1) const;
  void markRayUntilBlocked(int x0, int y0, int x1, int y1);
};
} // namespace core
