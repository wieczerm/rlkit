#include "../entities/Entity.hpp"
#include "../world/Tile.hpp"
#include "OpenAction.hpp"

namespace actions {

OpenAction::OpenAction(Entity &actor, Position target)
    : actor_(actor), target_(target) {}

ActionResult OpenAction::execute(world::Map &map) {
  // Check bounds
  if (!map.inBounds(target_)) {
    return ActionResult::invalid("Out of bounds");
  }

  // Check if it's a closed door
  auto tile = map.at(target_);

  if (tile == world::Tile::DoorClosed) {
    map.set(target_, world::Tile::DoorOpen);
    return ActionResult::success("Door opened", 100);
  }

  if (tile == world::Tile::DoorOpen) {
    return ActionResult::failure("Door already open");
  }

  return ActionResult::failure("Nothing to open here");
}

} // namespace actions
