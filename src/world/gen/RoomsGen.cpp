#include "RoomsGen.hpp"
#include <algorithm>
#include <vector>

namespace { // ---------- file-scope helpers (rooms) ----------

struct Rect {
  int x, y, w, h;
  Position center() const { return {x + w / 2, y + h / 2}; }
};

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

void carveLCorridorStop(world::Map &m, Position a, Position b,
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
void normalize_rooms_options(world::RoomsOptions &opt, const world::Map &m) {
  opt.room_min = std::max(2, opt.room_min);
  opt.room_max = std::max(opt.room_min, opt.room_max);

  const int maxW = std::max(2, m.width() - 2);
  const int maxH = std::max(2, m.height() - 2);
  const int hardMax = std::max(2, std::min(maxW, maxH));
  opt.room_max = std::min(opt.room_max, hardMax);

  const int avg = (opt.room_min + opt.room_max) / 2;
  const int avgArea = std::max(4, avg * avg);
  const int softCap = std::max(1, (m.width() * m.height()) / (avgArea + 8));
  if (opt.max_rooms <= 0)
    opt.max_rooms = 1;
  opt.max_rooms = std::min(opt.max_rooms, softCap);
}

} // namespace

namespace world {

void generateRoomsModule(Map &m, const RoomsOptions &optIn, std::mt19937 &G) {

  m.fill(Tile::Wall);

  RoomsOptions opt = optIn;
  normalize_rooms_options(opt, m);

  std::uniform_int_distribution<int> rw(opt.room_min, opt.room_max);
  std::uniform_int_distribution<int> rh(opt.room_min, opt.room_max);

  std::vector<Rect> rooms;
  rooms.reserve(static_cast<size_t>(opt.max_rooms));

  const int attempts = std::max(100, opt.max_rooms * 8);
  int placed = 0;

  // Placement loop with attempts budget.
  for (int i = 0; i < attempts && placed < opt.max_rooms; ++i) {
    int w = rw(G), h = rh(G);
    if (w >= m.width() - 2 || h >= m.height() - 2)
      continue;

    std::uniform_int_distribution<int> rx(1, std::max(1, m.width() - w - 2));
    std::uniform_int_distribution<int> ry(1, std::max(1, m.height() - h - 2));
    Rect r{rx(G), ry(G), w, h};

    // 1-tile buffer — rooms won’t merge into one blob.
    if (!areaIsAllWalls(m, r, /*pad=*/1))
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
}

} // namespace world
