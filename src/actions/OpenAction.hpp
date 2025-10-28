#pragma once
#include "../core/Position.hpp"
#include "../entities/Entity.hpp"
#include "../world/Map.hpp"
#include "ActionResult.hpp"

namespace entities {
class Entity;
}

namespace actions {

using entities::Entity;

class OpenAction {
public:
  OpenAction(Entity &actor, Position target);

  ActionResult execute(world::Map &map);

private:
  Entity &actor_;
  Position target_;
};

} // namespace actions
