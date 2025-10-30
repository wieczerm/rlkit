#pragma once

namespace core {
struct Position {
  int x;
  int y;

  constexpr Position() noexcept : x(0), y(0) {}
  constexpr Position(int x_, int y_) noexcept : x(x_), y(y_) {}

  constexpr bool operator==(const Position &other) const noexcept {
    return x == other.x && y == other.y;
  }
  constexpr bool operator!=(const Position &other) const noexcept {
    return !(*this == other);
  }
  constexpr Position operator+(const Position &other) const noexcept {
    return {x + other.x, y + other.y};
  }
  constexpr Position operator-(const Position &other) const noexcept {
    return {x - other.x, y - other.y};
  }
};
} // namespace core
