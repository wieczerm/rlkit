#include "EntityManager.hpp"
#include <algorithm>
#include <memory>

namespace entities {

void EntityManager::addEntity(std::unique_ptr<Entity> entity) {
  entities_.push_back(std::move(entity));
}

void EntityManager::removeEntity(Entity *entity) {
  entities_.erase(std::remove_if(entities_.begin(), entities_.end(),
                                 [entity](const std::unique_ptr<Entity> &e) {
                                   return e.get() == entity;
                                 }),
                  entities_.end());
}

Entity *EntityManager::getEntityAt(const core::Position &pos) const {
  for (const auto &entity : entities_) {
    if (entity->getPosition() == pos) {
      return entity.get();
    }
  }
  return nullptr;
}

std::vector<Entity *>
EntityManager::getEntitiesAt(const core::Position &pos) const {
  std::vector<Entity *> result;
  for (const auto &entity : entities_) {
    if (entity->getPosition() == pos) {
      result.push_back(entity.get());
    }
  }
  return result;
}

} // namespace entities
