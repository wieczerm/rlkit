#include "SimpleAI.hpp"
#include "actions/MoveAction.hpp"
#include "entities/Entity.hpp"
#include "entities/EntityManager.hpp"
#include "entities/TurnManager.hpp"
#include "world/Map.hpp"
#include "world/MapViewAdapter.hpp"

namespace ai {

SimpleAI::SimpleAI(int vision_range) : vision_range_(vision_range) {}

actions::ActionResult SimpleAI::act(entities::Entity &self,
                                    const entities::Entity &player,
                                    world::Map &map,
                                    world::FeatureManager &features,
                                    entities::EntityManager &entities,
                                    entities::TurnManager &turnMgr) {

  core::Position selfPos = self.getPosition();
  core::Position playerPos = player.getPosition();

  // Create fresh adapter and systems each time
  world::MapViewAdapter adapter(map);
  core::FOV fov(adapter);
  core::Pathfinding pathfinder(adapter);

  // Compute FOV from self position
  fov.compute(selfPos, vision_range_);

  // Can we see player?
  if (!fov.isVisible(playerPos.x, playerPos.y)) {
    return actions::ActionResult::success("", 100);
  }

  // Player visible - pathfind towards them
  auto path = pathfinder.findPath(selfPos, playerPos);

  if (path.empty() || path.size() < 2) {
    return actions::ActionResult::success("", 100);
  }

  // Move to next position
  core::Position nextPos = path[1];
  actions::MoveAction move(self, nextPos);
  return move.execute(map, features, entities, turnMgr);
}

} // namespace ai
