#pragma once
#include "../core/FOV.hpp"
#include "../core/Pathfinding.hpp"
#include "AIBehavior.hpp"
#include <memory>

namespace ai {

// Simple chase AI: if player visible → move towards, else → wait
class SimpleAI : public AIBehavior {
public:
  SimpleAI(int vision_range = 8);

  actions::ActionResult act(entities::Entity &self,
                            const entities::Entity &player, world::Map &map,
                            entities::EntityManager &entities,
                            entities::TurnManager &turnMgr) override;

private:
  int vision_range_;
  //  std::unique_ptr<FOV> fov_;
  //  std::unique_ptr<core::Pathfinding> pathfinder_;
};

} // namespace ai
