#include "TurnManager.hpp"
#include <algorithm>
#include <fstream>

namespace entities {

void TurnManager::addEntity(Entity *entity) {
  int speed = entity->getProperty("speed", 100);
  queue_.push({entity, 0}); // Start with 0 energy!
}

void TurnManager::removeEntity(Entity *entity) {
  std::priority_queue<TurnEntry> newQueue;
  while (!queue_.empty()) {
    TurnEntry entry = queue_.top();
    queue_.pop();
    if (entry.entity != entity) {
      newQueue.push(entry);
    }
  }
  queue_ = std::move(newQueue);
}

Entity *TurnManager::getNextActor() {
  if (queue_.empty())
    return nullptr;

  // Check if anyone can act (energy >= 100)
  if (queue_.top().energy < 100) {
    // No one ready - advance time for everyone
    std::vector<TurnEntry> all;
    while (!queue_.empty()) {
      TurnEntry entry = queue_.top();
      queue_.pop();

      int speed = entry.entity->getProperty("speed", 100);
      entry.energy += speed;

      all.push_back(entry);
    }

    // Re-insert all
    for (const auto &entry : all) {
      queue_.push(entry);
    }
  }

  return queue_.top().entity;
}

Entity *TurnManager::processTurn() {
  if (queue_.empty())
    return nullptr;

  TurnEntry current = queue_.top();
  queue_.pop();

  // Subtract action cost
  current.energy -= 100;

  // Re-add to queue
  queue_.push(current);

  return current.entity;
}

void TurnManager::clear() {
  while (!queue_.empty()) {
    queue_.pop();
  }
}

} // namespace entities
