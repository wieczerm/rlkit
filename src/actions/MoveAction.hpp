#pragma once
#include "ActionResult.hpp"
#include "core/Position.hpp"
#include "entities/Entity.hpp"
#include "entities/EntityManager.hpp"
#include "entities/TurnManager.hpp"
#include "world/Map.hpp"

namespace entities {
class Entity;
}
namespace world {
class FeatureManager;
}

namespace actions {

using entities::Entity;

class MoveAction {
public:
  MoveAction(Entity &actor, core::Position target);

  ActionResult execute(world::Map &map, world::FeatureManager &features,
                       entities::EntityManager &entities,
                       entities::TurnManager &turnMgr);

private:
  Entity &actor_;
  core::Position target_;
};

} // namespace actions
