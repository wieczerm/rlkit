#pragma once
#include "Position.hpp"

namespace core {

enum class InputAction {
  None,
  MoveNorth,
  MoveSouth,
  MoveEast,
  MoveWest,
  MoveNorthEast,
  MoveNorthWest,
  MoveSouthEast,
  MoveSouthWest,
  Wait,
  Open,
  Quit
};

class InputHandler {
public:
  static char readChar();
  InputHandler() = default;

  // Get input action (blocking)
  InputAction getAction();

  // Convert action to direction delta
  static Position actionToDirection(InputAction action);
};

} // namespace core
