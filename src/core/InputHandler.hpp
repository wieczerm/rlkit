#pragma once
#include "InputAction.hpp"
#include "Position.hpp"

namespace core {
class InputHandler {
public:
  // Just reads raw char - no mapping
  static char readChar();

  // Keep helper for direction conversion
  static core::Position actionToDirection(InputAction action);
};

} // namespace core
