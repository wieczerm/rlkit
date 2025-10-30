#include "MoveAction.hpp"
#include "systems/CombatSystem.hpp"
#include "world/FeatureManager.hpp"

namespace actions {

MoveAction::MoveAction(Entity &actor, core::Position target)
    : actor_(actor), target_(target) {}

ActionResult MoveAction::execute(world::Map &map,
                                 world::FeatureManager &features,
                                 entities::EntityManager &entities,
                                 entities::TurnManager &turnMgr) {
  // Check bounds
  if (!map.inBounds(target_)) {
    return ActionResult::invalid("Out of bounds");
  }

  // Check if blocked by terrain
  if (map.blocksMovement(target_)) {
    return ActionResult::failure("Blocked by wall");
  }

  // Check if blocked by features (closed doors)
  if (features.blocksMovement(target_)) {
    return ActionResult::failure("Door is closed");
  }

  // Check for entity at target
  Entity *target_entity = entities.getEntityAt(target_);

  if (target_entity) {
    // Bump attack!
    auto result = systems::CombatSystem::meleeAttack(actor_, *target_entity);
    //  Remove if killed
    if (result.killed) {
      entities.removeEntity(target_entity);
      turnMgr.removeEntity(target_entity);
    }

    return ActionResult::success(result.message, 100);
  }

  // Empty tile - move
  actor_.setPosition(target_);
  return ActionResult::success("", 100); // Silent movement
}

} // namespace actions
