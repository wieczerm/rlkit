#pragma once
#include "IMapView.hpp"
#include "Position.hpp"
#include <vector>

namespace core {

class Pathfinding {
public:
  explicit Pathfinding(const IMapView &map);

  // Finds path from start to goal using A*
  // Returns empty vector if no path exists
  std::vector<Position> findPath(const Position &start, const Position &goal);

private:
  const IMapView &map_;

  // Heuristic: Manhattan distance
  int heuristic(const Position &a, const Position &b) const;

  // Get walkable neighbors (8-directional)
  std::vector<Position> getNeighbors(const Position &pos) const;
};

} // namespace core
