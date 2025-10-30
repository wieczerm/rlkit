#pragma once

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
  Look,
  Descend,
  Quit
};

} // namespace core
