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

void placeStairsInRoom(world::Map &map, const Rect &room) {
  core::Position center = room.center();
  map.set(center, world::Tile::StairsDown);
}

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
      if (!world::isWall(m.at({x, y})))
        return false;
  return true;
}

void carveRect(world::Map &m, const Rect &r) {
  for (int y = r.y; y < r.y + r.h; ++y)
    for (int x = r.x; x < r.x + r.w; ++x)
      m.set({x, y}, world::Tile::Floor);
}

// Stop before entering a room interior so doors can be auto-placed later.
void digToEdgeH(world::Map &m, int x0, int x1, int y) {
  int dx = sgn(x1 - x0);
  for (int x = x0; x != x1 + dx; x += dx) {
    if (world::isFloor(m.at({x, y})))
      continue;
    int nx = x + dx;
    if (m.inBounds(nx, y) && world::isFloor(m.at({nx, y})))
      break;
    m.set({x, y}, world::Tile::Floor);
  }
}
void digToEdgeV(world::Map &m, int y0, int y1, int x) {
  int dy = sgn(y1 - y0);
  for (int y = y0; y != y1 + dy; y += dy) {
    if (world::isFloor(m.at({x, y})))
      continue;
    int ny = y + dy;
    if (m.inBounds(x, ny) && world::isFloor(m.at({x, ny})))
      break;
    m.set({x, y}, world::Tile::Floor);
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

// Try to place stairs using probability-based selection
bool tryPlaceStairsWithProbability(world::Map &map,
                                   std::vector<Rect> &availableRooms,
                                   std::mt19937 &rng) {
  if (availableRooms.empty())
    return false;

  std::uniform_real_distribution<double> dis(0.0, 1.0);

  for (size_t i = 0; i < availableRooms.size(); ++i) {
    double probability =
        static_cast<double>(i + 1) / static_cast<double>(availableRooms.size());

    if (dis(rng) < probability) {
      placeStairsInRoom(map, availableRooms[i]);
      availableRooms.erase(availableRooms.begin() +
                           static_cast<std::vector<Rect>::difference_type>(i));
      return true;
    }
  }

  return false;
}

// Place stairs with configurable probabilities
void placeStairsInRooms(world::Map &m, const std::vector<Rect> &rooms,
                        std::mt19937 &rng) {
  if (rooms.empty())
    return;

  std::vector<Rect> availableRooms = rooms; // Copy for mutation
  std::uniform_real_distribution<double> dis(0.0, 1.0);

  // First stairs - guaranteed (probability-based selection)
  tryPlaceStairsWithProbability(m, availableRooms, rng);

  // Second stairs - 15% chance
  if (dis(rng) < 0.15) {
    tryPlaceStairsWithProbability(m, availableRooms, rng);
  }

  // Third stairs - 5% chance
  if (dis(rng) < 0.05) {
    tryPlaceStairsWithProbability(m, availableRooms, rng);
  }
}

} // namespace

namespace world {

void generateRoomsModuleImpl(Map &m, const RoomsOptions &optIn,
                             const ::config::RoomPlacementConfig &placement,
                             std::mt19937 &G) {

  m.fill(Tile::Wall);

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

  if (!rooms.empty()) {
    placeStairsInRooms(m, rooms, G);
  }
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
