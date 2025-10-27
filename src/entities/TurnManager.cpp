#include "src/entities/TurnManager.hpp"
#include <algorithm>

namespace entities {

void TurnManager::addEntity(Entity *entity) {
  if (!entity)
    return;
  
  int speed = entity->getProperty("speed", 100);
  queue_.push({entity, speed});
}

void TurnManager::removeEntity(Entity *entity) {
  if (!entity)
    return;

  // Rebuild queue without the entity
  std::vector<TurnEntry> temp;
  while (!queue_.empty()) {
    TurnEntry entry = queue_.top();
    queue_.pop();
    if (entry.entity != entity) {
      temp.push_back(entry);
    }
  }

  for (const auto &entry : temp) {
    queue_.push(entry);
  }
}

Entity *TurnManager::getNextActor() {
  if (queue_.empty())
    return nullptr;
  return queue_.top().entity;
}

Entity *TurnManager::processTurn() {
  if (queue_.empty())
    return nullptr;

  TurnEntry current = queue_.top();
  queue_.pop();

  // Reduce energy and re-add if still has energy
  current.energy -= TURN_COST;
  if (current.energy > 0) {
    queue_.push(current);
  } else {
    // Refresh energy based on speed
    int speed = current.entity->getProperty("speed", 100);
    queue_.push({current.entity, speed});
  }

  return current.entity;
}

void TurnManager::clear() {
  while (!queue_.empty()) {
    queue_.pop();
  }
}

} // namespace entities
