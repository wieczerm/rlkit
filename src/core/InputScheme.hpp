#pragma once
#include "InputAction.hpp"
#include <string>
#include <unordered_map>

namespace core {

// Key binding map: char → InputAction
using KeyBindings = std::unordered_map<char, InputAction>;

enum class Scheme { Vi, WASD, Arrows };

class InputScheme {
public:
  static KeyBindings getScheme(Scheme scheme);
  static KeyBindings getViScheme();
  static KeyBindings getWASDScheme();
  static KeyBindings getArrowsScheme();
};

} // namespace core
