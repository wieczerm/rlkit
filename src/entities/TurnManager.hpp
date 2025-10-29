#pragma once
#include "Entity.hpp"
#include <queue>
#include <vector>

namespace entities {

// Helper struct for priority queue
struct TurnEntry {
  Entity *entity;
  int energy; // Higher = acts sooner

  bool operator<(const TurnEntry &other) const {
    return energy < other.energy; // Min-heap, so reverse for max behavior
  }
};

class TurnManager {
public:
  TurnManager() = default;

  // Add entity to turn queue (uses "speed" property, default 100)
  void addEntity(Entity *entity);

  // Remove entity from queue
  void removeEntity(Entity *entity);

  // Get next entity to act (returns nullptr if queue empty)
  Entity *getNextActor();

  // Advance time - processes turn and returns acting entity
  Entity *processTurn();

  // Check if queue is empty
  bool isEmpty() const noexcept { return queue_.empty(); }

  // Clear all entities
  void clear();

private:
  std::priority_queue<TurnEntry> queue_;

  // Energy cost per turn (can be based on action type later)
  static constexpr int TURN_COST = 10;
};

} // namespace entities
