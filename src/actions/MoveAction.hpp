#pragma once
#include "../core/Position.hpp"
#include "../entities/Entity.hpp"
#include "../entities/EntityManager.hpp"
#include "../entities/TurnManager.hpp"
#include "../world/Map.hpp"
#include "ActionResult.hpp"

namespace entities {
class Entity;
}

namespace actions {

using entities::Entity;

class MoveAction {
public:
  MoveAction(Entity &actor, Position target);

  ActionResult execute(world::Map &map, entities::EntityManager &entities,
                       entities::TurnManager &turnMgr);

private:
  Entity &actor_;
  Position target_;
};

} // namespace actions
