#include "InputHandler.hpp"
#include "Position.hpp"
#include <iostream>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

namespace core {

char InputHandler::readChar() {
#ifdef _WIN32
  // Windows: use _getch() for immediate input
  return static_cast<char>(_getch());
#else
  // Unix/Linux: set terminal to raw mode
  struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  char ch = getchar();

  // Restore terminal
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return ch;
#endif
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
