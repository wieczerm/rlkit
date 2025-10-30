#include "RoomsGen.hpp"
#include "config/DungeonConfig.hpp"
#include "world/Map.hpp"
#include "world/Tile.hpp"
#include <algorithm>
#include <iostream>
#include <vector>

namespace { // ---------- file-scope helpers (rooms) ----------

struct Rect {
  int x, y, w, h;
  core::Position center() const { return {x + w / 2, y + h / 2}; }
};

// REMOVED: Stairs are now placed by FeaturePlacer via Feature system
// void placeStairsInRoom(world::Map &map, const Rect &room) {
//   core::Position center = room.center();
//   map.set(center, world::Tile::StairsDown);
// }

inline int sgn(int v) { return (v > 0) - (v < 0); }

// “Gap check”: room area plus `pad` cells must be all walls.
// Prevents overlaps and keeps a one-tile gap between rooms.
bool areaIsAllWalls(const world::Map &m, const Rect &r, int pad) {
  const int x0 = std::max(0, r.x - pad);
  const int y0 = std::max(0, r.y - pad);
  const int x1 = std::min(m.width(), r.x + r.w + pad);
  const int y1 = std::min(m.height(), r.y + r.h + pad);
  for (int y = y0; y < y1; ++y)
    for (int x = x0; x < x1; ++x)
      if (m.at({x, y}) != world::Tile::SolidRock)
        return false;
  return true;
}

void carveRect(world::Map &m, const Rect &r) {
  for (int y = r.y; y < r.y + r.h; ++y)
    for (int x = r.x; x < r.x + r.w; ++x)
      m.set({x, y}, world::Tile::OpenGround);
}

// Stop before entering a room interior so doors can be auto-placed later.
void digToEdgeH(world::Map &m, int x0, int x1, int y) {
  int dx = sgn(x1 - x0);
  for (int x = x0; x != x1 + dx; x += dx) {
    if (m.at({x, y}) == world::Tile::OpenGround)
      continue;
    int nx = x + dx;
    if (m.inBounds(nx, y) && (m.at({nx, y}) == world::Tile::OpenGround))
      break;
    m.set({x, y}, world::Tile::OpenGround);
  }
}
void digToEdgeV(world::Map &m, int y0, int y1, int x) {
  int dy = sgn(y1 - y0);
  for (int y = y0; y != y1 + dy; y += dy) {
    if (m.at({x, y}) == world::Tile::OpenGround)
      continue;
    int ny = y + dy;
    if (m.inBounds(x, ny) && m.at({x, ny}) == world::Tile::OpenGround)
      break;
    m.set({x, y}, world::Tile::OpenGround);
  }
}

void carveLCorridorStop(world::Map &m, core::Position a, core::Position b,
                        std::mt19937 &G) {
  if (a.x == b.x && a.y == b.y)
    return;
  if (std::uniform_int_distribution<int>(0, 1)(G) == 0) {
    digToEdgeH(m, a.x, b.x, a.y);
    digToEdgeV(m, a.y, b.y, b.x);
  } else {
    digToEdgeV(m, a.y, b.y, a.x);
    digToEdgeH(m, a.x, b.x, b.y);
  }
}

// Clamp and normalize options to the map size.
void normalize_rooms_options(world::RoomsOptions &opt,
                             const ::config::RoomPlacementConfig &placement,
                             const world::Map &m) {
  opt.room_min = std::max(placement.min_room_dimension, opt.room_min);
  opt.room_max = std::max(opt.room_min, opt.room_max);

  const int maxW =
      std::max(placement.edge_margin, m.width() - placement.edge_margin);
  const int maxH =
      std::max(placement.edge_margin, m.height() - placement.edge_margin);
  const int hardMax =
      std::max(placement.min_room_dimension, std::min(maxW, maxH));
  opt.room_max = std::min(opt.room_max, hardMax);

  const int avg = (opt.room_min + opt.room_max) / 2;
  const int avgArea = std::max(4, avg * avg);
  const int softCap = std::max(1, (m.width() * m.height()) / (avgArea + 8));
  if (opt.max_rooms <= 0)
    opt.max_rooms = 1;
  opt.max_rooms = std::min(opt.max_rooms, softCap);
}

// REMOVED: Stairs placement moved to FeaturePlacer
// bool tryPlaceStairsWithProbability(...) { ... }

// REMOVED: Stairs placement moved to FeaturePlacer
// void placeStairsInRooms(...) { ... }

} // namespace

namespace world {

void generateRoomsModuleImpl(Map &m, const RoomsOptions &optIn,
                             const ::config::RoomPlacementConfig &placement,
                             std::mt19937 &G) {

  m.fill(Tile::SolidRock);

  RoomsOptions opt = optIn;
  normalize_rooms_options(opt, placement, m);

  std::uniform_int_distribution<int> rw(opt.room_min, opt.room_max);
  std::uniform_int_distribution<int> rh(opt.room_min, opt.room_max);

  std::vector<Rect> rooms;
  rooms.reserve(static_cast<size_t>(opt.max_rooms));

  const int attempts = std::max(placement.attempts_multiplier,
                                opt.max_rooms * placement.attempts_per_room);
  int placed = 0;

  // Placement loop with attempts budget.
  for (int i = 0; i < attempts && placed < opt.max_rooms; ++i) {
    int w = rw(G), h = rh(G);
    if (w >= m.width() - placement.edge_margin ||
        h >= m.height() - placement.edge_margin)
      continue;

    std::uniform_int_distribution<int> rx(
        1, std::max(1, m.width() - w - placement.edge_margin));
    std::uniform_int_distribution<int> ry(
        1, std::max(1, m.height() - h - placement.edge_margin));
    Rect r{rx(G), ry(G), w, h};

    // 1-tile buffer — rooms won’t merge into one blob.
    if (!areaIsAllWalls(m, r, placement.room_padding))
      continue;

    carveRect(m, r);
    rooms.push_back(r);
    ++placed;
  }

  if (rooms.empty())
    return;

  // Connect rooms (sorted) with corridors that stop at walls.
  std::sort(rooms.begin(), rooms.end(),
            [](const Rect &a, const Rect &b) { return a.x < b.x; });
  for (size_t i = 1; i < rooms.size(); ++i)
    carveLCorridorStop(m, rooms[i - 1].center(), rooms[i].center(), G);

  // Stairs are now placed by FeaturePlacer, not here
}
// New overload: accepts LevelConfig
void generateRoomsModule(Map &m, const config::LevelConfig &levelCfg,
                         std::mt19937 &G) {
  generateRoomsModuleImpl(m, levelCfg.rooms, levelCfg.room_placement, G);
}

// Old overload: backward compatibility wrapper
void generateRoomsModule(Map &m, const RoomsOptions &opt, std::mt19937 &G) {
  config::LevelConfig cfg;
  cfg.rooms = opt;
  generateRoomsModule(m, cfg, G);
}
} // namespace world
