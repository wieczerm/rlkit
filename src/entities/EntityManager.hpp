#pragma once
#include "Entity.hpp"
#include "core/Position.hpp"
#include <memory>
#include <vector>

namespace entities {

class EntityManager {
public:
  EntityManager() = default;

  // Add entity (takes ownership)
  void addEntity(std::unique_ptr<Entity> entity);

  // Remove entity by pointer
  void removeEntity(Entity *entity);

  // Get entity at position (returns first found, or nullptr)
  Entity *getEntityAt(const core::Position &pos) const;

  // Get all entities at position
  std::vector<Entity *> getEntitiesAt(const core::Position &pos) const;

  // Get all entities
  const std::vector<std::unique_ptr<Entity>> &getEntities() const noexcept {
    return entities_;
  }

  // Clear all entities
  void clear() noexcept { entities_.clear(); }

  // Get entity count
  size_t count() const noexcept { return entities_.size(); }

private:
  std::vector<std::unique_ptr<Entity>> entities_;
};

} // namespace entities
