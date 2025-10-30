#pragma once
#include "ActionResult.hpp"
#include "core/Position.hpp"
#include "entities/Entity.hpp"
#include "world/Map.hpp"

namespace entities {
class Entity;
}

namespace actions {

using entities::Entity;

class OpenAction {
public:
  OpenAction(Entity &actor, core::Position target);

  ActionResult execute(world::Map &map);

private:
  Entity &actor_;
  core::Position target_;
};

} // namespace actions
