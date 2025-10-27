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
  Quit
};

class InputHandler {
public:
  InputHandler() = default;

  // Get input action (blocking)
  InputAction getAction();

  // Convert action to direction delta
  static Position actionToDirection(InputAction action);

private:
  // Read single character from stdin
  char readChar();
};

} // namespace core
