#pragma once
#include <cstdint>

namespace world {

enum class Tile : std::uint8_t {
  Floor,
  Wall,
  DoorClosed,
  DoorOpen,
};

inline constexpr bool blocksMovement(Tile t) noexcept {
  switch (t) {
  case Tile::Wall:
    return true;
  case Tile::DoorClosed:
    return true;
  case Tile::Floor:
    return false;
  case Tile::DoorOpen:
    return false;
  }
  return true;
}

inline constexpr bool blocksLineOfSight(Tile t) noexcept {
  switch (t) {
  case Tile::Wall:
    return true;
  case Tile::DoorClosed:
    return true;
  case Tile::Floor:
    return false;
  case Tile::DoorOpen:
    return false;
  }
  return true;
}

inline bool isDoor(Tile t) {
  return t == Tile::DoorClosed || t == Tile::DoorOpen;
}
inline bool isWall(Tile t) { return t == Tile::Wall; }
inline bool isFloor(Tile t) {
  return t == Tile::Floor || t == Tile::DoorOpen || t == Tile::DoorClosed;
}
inline bool isDoorClosed(Tile t) { return t == Tile::DoorClosed; }
inline bool isDoorOpen(Tile t) { return t == Tile::DoorOpen; }

} // namespace world
