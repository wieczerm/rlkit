#include "src/core/Pathfinding.hpp"
#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_map>
#include <unordered_set>

namespace core {

// Hash function for Position
struct PositionHash {
  std::size_t operator()(const Position &p) const noexcept {
    return static_cast<std::size_t>(p.x) * 73856093u ^
           static_cast<std::size_t>(p.y) * 19349663u;
  }
};

// Node for A* priority queue
struct Node {
  Position pos;
  int f; // f = g + h
  int g; // cost from start

  bool operator>(const Node &other) const { return f > other.f; }
};

Pathfinding::Pathfinding(const IMapView &map) : map_(map) {}

int Pathfinding::heuristic(const Position &a, const Position &b) const {
  return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

std::vector<Position> Pathfinding::getNeighbors(const Position &pos) const {
  std::vector<Position> neighbors;
  neighbors.reserve(8);

  // 8 directions: N, NE, E, SE, S, SW, W, NW
  static const int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
  static const int dy[] = {-1, -1, 0, 1, 1, 1, 0, -1};

  for (int i = 0; i < 8; ++i) {
    int nx = pos.x + dx[i];
    int ny = pos.y + dy[i];

    if (nx >= 0 && nx < map_.width() && ny >= 0 && ny < map_.height()) {
      if (!map_.blocksLineOfSight(nx, ny)) {
        neighbors.push_back({nx, ny});
      }
    }
  }

  return neighbors;
}

std::vector<Position> Pathfinding::findPath(const Position &start,
                                             const Position &goal) {
  if (start == goal)
    return {start};

  if (map_.blocksLineOfSight(goal.x, goal.y))
    return {};

  std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
  std::unordered_set<Position, PositionHash> closedSet;
  std::unordered_map<Position, Position, PositionHash> cameFrom;
  std::unordered_map<Position, int, PositionHash> gScore;

  gScore[start] = 0;
  openSet.push({start, heuristic(start, goal), 0});

  while (!openSet.empty()) {
    Node current = openSet.top();
    openSet.pop();

    if (current.pos == goal) {
      // Reconstruct path
      std::vector<Position> path;
      Position p = goal;
      while (p != start) {
        path.push_back(p);
        p = cameFrom[p];
      }
      path.push_back(start);
      std::reverse(path.begin(), path.end());
      return path;
    }

    if (closedSet.count(current.pos))
      continue;
    closedSet.insert(current.pos);

    for (const Position &neighbor : getNeighbors(current.pos)) {
      if (closedSet.count(neighbor))
        continue;

      int tentativeG = current.g + 1;

      if (!gScore.count(neighbor) || tentativeG < gScore[neighbor]) {
        gScore[neighbor] = tentativeG;
        cameFrom[neighbor] = current.pos;
        int h = heuristic(neighbor, goal);
        openSet.push({neighbor, tentativeG + h, tentativeG});
      }
    }
  }

  return {}; // No path found
}

} // namespace core
