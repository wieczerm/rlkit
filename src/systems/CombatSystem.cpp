#include "CombatSystem.hpp"
#include <algorithm>
#include <sstream>

namespace systems {

int CombatSystem::calculateDamage(int str, float phys_res) {
  // Clamp phys_res to [0.0, 0.75]
  phys_res = std::clamp(phys_res, 0.0f, 0.75f);

  // HP = HP - STR * (1 - phys_res)
  int damage = static_cast<int>(str * (1.0f - phys_res));
  return std::max(1, damage); // Minimum 1 damage
}

CombatSystem::AttackResult CombatSystem::meleeAttack(Entity &attacker,
                                                     Entity &defender) {
  int str = attacker.getProperty("str");
  float phys_res = static_cast<float>(defender.getProperty("phys_res")) /
                   100.0f; // Assume stored as percentage

  int damage = calculateDamage(str, phys_res);

  int current_hp = defender.getProperty("hp");
  int new_hp = current_hp - damage;
  defender.setProperty("hp", new_hp);

  bool killed = (new_hp <= 0);

  std::stringstream msg;
  msg << attacker.getName() << " hits " << defender.getName() << " for "
      << damage << " damage";
  if (killed) {
    msg << " (killed)";
  }

  return {damage, killed, msg.str()};
}

} // namespace systems
