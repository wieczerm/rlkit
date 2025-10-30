#pragma once
#include "Tile.hpp"
#include "TileEnum.hpp"

namespace world {

// Properties defining tile behavior and characteristics
struct TileProperties {
  int movement_cost;    // 100 = normal speed, 200 = half speed, -1 = impassable
  bool blocks_los;      // Does this tile block line of sight?
  int damage_immediate; // Damage dealt when stepping onto tile
  int damage_per_turn;  // Damage dealt each turn while standing on tile

  // Default constructor for safe initialization
  TileProperties()
      : movement_cost(-1), blocks_los(true), damage_immediate(0),
        damage_per_turn(0) {}

  // Parameterized constructor
  TileProperties(int move_cost, bool blocks, int dmg_imm, int dmg_turn)
      : movement_cost(move_cost), blocks_los(blocks), damage_immediate(dmg_imm),
        damage_per_turn(dmg_turn) {}
};

// Convenience wrapper function for accessing tile properties
// Internally queries TileRegistry singleton
const TileProperties &getProperties(Tile t);

// Helper functions for common property queries
inline bool blocksMovement(Tile t) {
  return getProperties(t).movement_cost < 0;
}

inline bool blocksLineOfSight(Tile t) { return getProperties(t).blocks_los; }

inline int getMovementCost(Tile t) { return getProperties(t).movement_cost; }

inline int getImmediateDamage(Tile t) {
  return getProperties(t).damage_immediate;
}

inline int getDamagePerTurn(Tile t) { return getProperties(t).damage_per_turn; }

} // namespace world
