#include "FOV.hpp"
#include <algorithm>
#include <cmath>

namespace core {

FOV::FOV(const core::IMapView &map)
    : map_(map), width_(static_cast<size_t>(map.width())),
      height_(static_cast<size_t>(map.height())),
      visible_(height_, std::vector<bool>(width_, false)) {}

void FOV::compute(const Position &origin, int radius) {
  for (auto &row : visible_)
    std::fill(row.begin(), row.end(), false);
  visible_[static_cast<std::size_t>(origin.y)]
          [static_cast<std::size_t>(origin.x)] = true;

  for (int dy = -radius; dy <= radius; ++dy) {
    for (int dx = -radius; dx <= radius; ++dx) {
      if (std::max(std::abs(dx), std::abs(dy)) > radius)
        continue;

      int x = origin.x + dx;
      int y = origin.y + dy;

      if (x < 0 || static_cast<size_t>(x) >= width_ || y < 0 ||
          static_cast<size_t>(y) >= height_)
        continue;

      // NEW: Mark all tiles along the ray until first blocker
      markRayUntilBlocked(origin.x, origin.y, x, y);
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

void FOV::markRayUntilBlocked(int x0, int y0, int x1, int y1) {
  // Bresenham - mark all tiles until we hit a blocker
  int dx = std::abs(x1 - x0);
  int dy = std::abs(y1 - y0);
  int sx = x0 < x1 ? 1 : -1;
  int sy = y0 < y1 ? 1 : -1;
  int err = dx - dy;
  int x = x0;
  int y = y0;

  while (true) {
    // Mark current tile as visible
    visible_[static_cast<size_t>(y)][static_cast<size_t>(x)] = true;

    // If this tile blocks, stop here
    if (map_.blocksLineOfSight(x, y))
      return;

    // Reached target
    if (x == x1 && y == y1)
      return;

    // Bresenham step
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
}

} // namespace core
