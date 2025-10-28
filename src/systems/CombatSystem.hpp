#pragma once
#include "../entities/Entity.hpp"
#include <string>

namespace entities {
class Entity;
}

namespace systems {

using entities::Entity;

class CombatSystem {
public:
  // Returns damage dealt and message
  struct AttackResult {
    int damage;
    bool killed;
    std::string message;
  };

  static AttackResult meleeAttack(Entity &attacker, Entity &defender);

private:
  static int calculateDamage(int str, float phys_res);
};

} // namespace systems
