#include "src/core/InputHandler.hpp"
#include <iostream>
#include <termios.h>
#include <unistd.h>

namespace core {

char InputHandler::readChar() {
  // Set terminal to raw mode
  struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  char ch = getchar();

  // Restore terminal
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return ch;
}

InputAction InputHandler::getAction() {
  char ch = readChar();

  switch (ch) {
  // Vi keys
  case 'k':
    return InputAction::MoveNorth;
  case 'j':
    return InputAction::MoveSouth;
  case 'h':
    return InputAction::MoveWest;
  case 'l':
    return InputAction::MoveEast;
  case 'y':
    return InputAction::MoveNorthWest;
  case 'u':
    return InputAction::MoveNorthEast;
  case 'b':
    return InputAction::MoveSouthWest;
  case 'n':
    return InputAction::MoveSouthEast;

  // Arrow keys (WASD)
  case 'w':
    return InputAction::MoveNorth;
  case 's':
    return InputAction::MoveSouth;
  case 'a':
    return InputAction::MoveWest;
  case 'd':
    return InputAction::MoveEast;

  // Wait
  case '.':
  case ' ':
    return InputAction::Wait;

  // Quit
  case 'q':
  case 'Q':
    return InputAction::Quit;

  default:
    return InputAction::None;
  }
}

Position InputHandler::actionToDirection(InputAction action) {
  switch (action) {
  case InputAction::MoveNorth:
    return {0, -1};
  case InputAction::MoveSouth:
    return {0, 1};
  case InputAction::MoveEast:
    return {1, 0};
  case InputAction::MoveWest:
    return {-1, 0};
  case InputAction::MoveNorthEast:
    return {1, -1};
  case InputAction::MoveNorthWest:
    return {-1, -1};
  case InputAction::MoveSouthEast:
    return {1, 1};
  case InputAction::MoveSouthWest:
    return {-1, 1};
  default:
    return {0, 0};
  }
}

} // namespace core
