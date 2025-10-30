#include "InputScheme.hpp"

namespace core {

KeyBindings InputScheme::getScheme(Scheme scheme) {
  switch (scheme) {
  case Scheme::Vi:
    return getViScheme();
  case Scheme::WASD:
    return getWASDScheme();
  case Scheme::Arrows:
    return getArrowsScheme();
  default:
    return getViScheme();
  }
}

KeyBindings InputScheme::getViScheme() {
  return {// Movement (vi keys)
          {'h', InputAction::MoveWest},
          {'j', InputAction::MoveSouth},
          {'k', InputAction::MoveNorth},
          {'l', InputAction::MoveEast},
          {'y', InputAction::MoveNorthWest},
          {'u', InputAction::MoveNorthEast},
          {'b', InputAction::MoveSouthWest},
          {'n', InputAction::MoveSouthEast},
          {'>', InputAction::Descend},

          // Actions
          {'.', InputAction::Wait},
          {'o', InputAction::Open},
          {'x', InputAction::Look},
          {'q', InputAction::Quit},

          // Uppercase variants
          {'H', InputAction::MoveWest},
          {'J', InputAction::MoveSouth},
          {'K', InputAction::MoveNorth},
          {'L', InputAction::MoveEast},
          {'Y', InputAction::MoveNorthWest},
          {'U', InputAction::MoveNorthEast},
          {'B', InputAction::MoveSouthWest},
          {'N', InputAction::MoveSouthEast},
          {'O', InputAction::Open},
          {'X', InputAction::Look},
          {'Q', InputAction::Quit}};
}

KeyBindings InputScheme::getWASDScheme() {
  return {// Movement (WASD)
          {'a', InputAction::MoveWest},
          {'s', InputAction::MoveSouth},
          {'w', InputAction::MoveNorth},
          {'d', InputAction::MoveEast},
          {'q', InputAction::MoveNorthWest},
          {'e', InputAction::MoveNorthEast},
          {'z', InputAction::MoveSouthWest},
          {'c', InputAction::MoveSouthEast},
          {'>', InputAction::Descend},

          // Actions
          {' ', InputAction::Wait},
          {'o', InputAction::Open},
          {'x', InputAction::Look},
          {'Q', InputAction::Quit}, // Shift+Q to quit

          // Uppercase variants
          {'A', InputAction::MoveWest},
          {'S', InputAction::MoveSouth},
          {'W', InputAction::MoveNorth},
          {'D', InputAction::MoveEast},
          {'O', InputAction::Open},
          {'X', InputAction::Look}};
}

KeyBindings InputScheme::getArrowsScheme() {
  return {// Note: Arrow keys are multi-byte sequences
          // For now, provide numpad as fallback
          {'4', InputAction::MoveWest},
          {'2', InputAction::MoveSouth},
          {'8', InputAction::MoveNorth},
          {'6', InputAction::MoveEast},
          {'7', InputAction::MoveNorthWest},
          {'9', InputAction::MoveNorthEast},
          {'1', InputAction::MoveSouthWest},
          {'3', InputAction::MoveSouthEast},
          {'>', InputAction::Descend},

          // Actions
          {'.', InputAction::Wait},
          {'o', InputAction::Open},
          {'x', InputAction::Look},
          {'q', InputAction::Quit},

          {'O', InputAction::Open},
          {'X', InputAction::Look},
          {'Q', InputAction::Quit}};
}

} // namespace core
