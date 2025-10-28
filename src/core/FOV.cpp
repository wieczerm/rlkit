#include "src/core/FOV.hpp"
#include <algorithm>
#include <cmath>

FOV::FOV(const core::IMapView &map)
    : map_(map), width_(static_cast<size_t>(map.width())),
      height_(static_cast<size_t>(map.height())),
      visible_(height_, std::vector<bool>(width_, false)) {}

void FOV::compute(const Position &origin, int radius) {
  for (auto &row : visible_)
    std::fill(row.begin(), row.end(), false);

  visible_[static_cast<std::size_t>(origin.y)]
          [static_cast<std::size_t>(origin.x)] = true;

  // Simple circle-based FOV with line-of-sight checks
  for (int dy = -radius; dy <= radius; ++dy) {
    for (int dx = -radius; dx <= radius; ++dx) {
      // Skip if outside radius (Chebyshev distance)
      if (std::max(std::abs(dx), std::abs(dy)) > radius)
        continue;

      int x = origin.x + dx;
      int y = origin.y + dy;

      if (x < 0 || static_cast<size_t>(x) >= width_ || 
          y < 0 || static_cast<size_t>(y) >= height_)
        continue;

      // Check line of sight using Bresenham
      if (hasLineOfSight(origin.x, origin.y, x, y)) {
        visible_[static_cast<size_t>(y)][static_cast<size_t>(x)] = true;
      }
    }
  }
}

bool FOV::isVisible(int x, int y) const {
  if (x < 0 || static_cast<std::size_t>(x) >= width_ || y < 0 ||
      static_cast<std::size_t>(y) >= height_)
    return false;
  return visible_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
}

bool FOV::hasLineOfSight(int x0, int y0, int x1, int y1) const {
  // Bresenham's line algorithm for LOS check
  int dx = std::abs(x1 - x0);
  int dy = std::abs(y1 - y0);
  int sx = x0 < x1 ? 1 : -1;
  int sy = y0 < y1 ? 1 : -1;
  int err = dx - dy;

  int x = x0;
  int y = y0;

  while (true) {
    // Don't check blocking at the target position
    if (x == x1 && y == y1)
      return true;

    // Check if current position blocks LOS
    if (map_.blocksLineOfSight(x, y))
      return false;

    if (x == x1 && y == y1)
      break;

    int e2 = 2 * err;
    if (e2 > -dy) {
      err -= dy;
      x += sx;
    }
    if (e2 < dx) {
      err += dx;
      y += sy;
    }
  }

  return true;
}
