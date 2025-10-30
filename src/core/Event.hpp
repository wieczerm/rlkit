#pragma once
#include "core/Position.hpp"
#include <cstdint>
#include <string>
#include <variant>

namespace core {

// Priority levels for event processing (lower number = higher priority)
enum class EventPriority : std::uint8_t {
  Immediate = 0, // State changes: death, level transitions
  High = 1,      // Combat: damage, status effects
  Normal = 2,    // Interactions: movement, door operations
  Low = 3,       // UI updates (reserved for future use)
  Deferred = 4   // Cosmetic effects (reserved for future use)
};

// Entity death event - triggered when entity HP reaches 0
struct EntityDiedEvent {
  static constexpr EventPriority priority = EventPriority::Immediate;

  int entity_id;
  std::string name;
  Position position;
  int killer_id;              // -1 if environmental damage or no killer
  std::string cause_of_death; // "goblin", "lava", "starvation", etc.
};

// Entity damage event - triggered when entity takes damage
struct EntityDamagedEvent {
  static constexpr EventPriority priority = EventPriority::High;

  int entity_id;
  int attacker_id; // -1 if environmental damage
  int damage;
  int remaining_hp;
  std::string damage_type; // "physical", "fire", "poison", "acid", etc.
};

// Entity movement event - triggered when entity changes position
struct EntityMovedEvent {
  static constexpr EventPriority priority = EventPriority::Normal;

  int entity_id;
  Position from;
  Position to;
};

// Level transition event - triggered when player uses stairs
struct LevelTransitionEvent {
  static constexpr EventPriority priority = EventPriority::Immediate;

  enum Direction { Down, Up };

  Direction direction;
  int from_depth;
  int to_depth;
  Position stairs_position;
};

// Test event for unit testing
struct TestEvent {
  static constexpr EventPriority priority = EventPriority::Normal;

  int value;
  std::string label;
};

// Event variant - add new event types here
using Event = std::variant<EntityDiedEvent, EntityDamagedEvent,
                           EntityMovedEvent, LevelTransitionEvent, TestEvent>;

// Get priority of an event using visitor pattern
EventPriority getPriority(const Event &e);

} // namespace core
