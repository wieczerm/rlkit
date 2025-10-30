#include "OpenAction.hpp"
#include "entities/Entity.hpp"
#include "world/Feature.hpp"
#include "world/FeatureManager.hpp"
#include "world/FeatureProperties.hpp"
#include "world/Tile.hpp"

namespace actions {

OpenAction::OpenAction(Entity &actor, core::Position target)
    : actor_(actor), target_(target) {}

ActionResult OpenAction::execute(world::Map &map,
                                 world::FeatureManager &features) {
  // Check bounds
  if (!map.inBounds(target_)) {
    return ActionResult::invalid("Out of bounds");
  }

  // Check if there's a door feature at target
  world::Feature *feature = features.getFeature(target_);
  
  if (!feature) {
    return ActionResult::failure("Nothing to open here");
  }

  // Check if it's a door
  if (!world::isDoor(*feature)) {
    return ActionResult::failure("Nothing to open here");
  }

  world::Door *door = world::getDoor(*feature);
  
  if (door->state == world::Door::State::Open) {
    return ActionResult::failure("Door already open");
  }

  // Open the door
  door->state = world::Door::State::Open;
  return ActionResult::success("Door opened", 100);
}

} // namespace actions
