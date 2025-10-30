#pragma once
#include "actions/ActionResult.hpp"
#include "entities/TurnManager.hpp"

// Forward declarations
namespace entities {
class Entity;
class EntityManager;
class TurnManager;
} // namespace entities
namespace world {
class Map;
class FeatureManager;
}

namespace ai {

class AIBehavior {
public:
  virtual ~AIBehavior() = default;

  // Returns action result after AI decides and acts
  virtual actions::ActionResult
  act(entities::Entity &self, const entities::Entity &player, world::Map &map,
      world::FeatureManager &features, entities::EntityManager &entities,
      entities::TurnManager &turnMgr) = 0;
};

} // namespace ai
