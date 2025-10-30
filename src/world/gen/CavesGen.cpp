#include "CavesGen.hpp"
#include "config/DungeonConfig.hpp"
#include <algorithm>
#include <climits>
#include <queue>
#include <vector>

namespace { // ---------- file-scope helpers (caves) ----------

using CompId = size_t;
constexpr CompId Invalid = static_cast<CompId>(-1);

int countWallNeighbors8(const world::Map &m, int x, int y) {
  int c = 0;
  for (int yy = y - 1; yy <= y + 1; ++yy)
    for (int xx = x - 1; xx <= x + 1; ++xx) {
      if (xx == x && yy == y)
        continue;
      if (!m.inBounds(xx, yy) || world::isWall(m.at({xx, yy})))
        ++c;
    }
  return c;
}

// One CA iteration with birth/survive thresholds.
void caStep(world::Map &m, int birth, int survive) {
  world::Map next(m.width(), m.height(), world::Tile::Wall);
  for (int y = 0; y < m.height(); ++y)
    for (int x = 0; x < m.width(); ++x) {
      const bool edge =
          (x == 0 || y == 0 || x == m.width() - 1 || y == m.height() - 1);
      if (edge) {
        next.set({x, y}, world::Tile::Wall);
        continue;
      }
      const int wn = countWallNeighbors8(m, x, y);
      const bool wall_next =
          world::isWall(m.at({x, y})) ? (wn >= survive) : (wn >= birth);
      next.set({x, y}, wall_next ? world::Tile::Wall : world::Tile::Floor);
    }
  // copy back
  for (int y = 0; y < m.height(); ++y)
    for (int x = 0; x < m.width(); ++x)
      m.set({x, y}, next.at({x, y}));
}

// Label 4-neighbour floor components; returns count. `lbl` sized to W*H.
size_t labelComponents(const world::Map &m, std::vector<CompId> &lbl) {
  const int W = m.width(), H = m.height();
  lbl.assign(static_cast<size_t>(W) * static_cast<size_t>(H), Invalid);
  auto id = [&](int x, int y) {
    return static_cast<size_t>(y) * static_cast<size_t>(W) +
           static_cast<size_t>(x);
  };

  size_t comps = 0;
  std::queue<core::Position> q;
  for (int y = 0; y < H; ++y)
    for (int x = 0; x < W; ++x) {
      const auto k = id(x, y);
      if (!world::isFloor(m.at({x, y})) || lbl[k] != Invalid)
        continue;

      lbl[k] = comps;
      q.push({x, y});
      while (!q.empty()) {
        auto p = q.front();
        q.pop();
        static const int dx[4] = {1, -1, 0, 0}, dy[4] = {0, 0, 1, -1};
        for (int i = 0; i < 4; ++i) {
          const int nx = p.x + dx[i], ny = p.y + dy[i];
          if (!m.inBounds(nx, ny))
            continue;
          const auto kk = id(nx, ny);
          if (!world::isFloor(m.at({nx, ny})) || lbl[kk] != Invalid)
            continue;
          lbl[kk] = comps;
          q.push({nx, ny});
        }
      }
      ++comps;
    }
  return comps;
}

// Simple L corridor through walls between two points.
void carveLCorridorThroughWalls(world::Map &m, core::Position a,
                                core::Position b) {
  if (a.x == b.x && a.y == b.y)
    return;
  if (a.x <= b.x) {
    for (int x = std::min(a.x, b.x); x <= std::max(a.x, b.x); ++x)
      m.set({x, a.y}, world::Tile::Floor);
    for (int y = std::min(a.y, b.y); y <= std::max(a.y, b.y); ++y)
      m.set({b.x, y}, world::Tile::Floor);
  } else {
    for (int y = std::min(a.y, b.y); y <= std::max(a.y, b.y); ++y)
      m.set({a.x, y}, world::Tile::Floor);
    for (int x = std::min(a.x, b.x); x <= std::max(a.x, b.x); ++x)
      m.set({x, b.y}, world::Tile::Floor);
  }
}

// Connect every component to the largest one by shortest (Manhattan) L tunnels.
void connectComponentsToMain(world::Map &m) {
  const int W = m.width(), H = m.height();
  auto id = [&](int x, int y) {
    return static_cast<size_t>(y) * static_cast<size_t>(W) +
           static_cast<size_t>(x);
  };

  std::vector<CompId> lbl;
  size_t comps = labelComponents(m, lbl);
  if (comps <= 1)
    return;

  // Find main component by size.
  std::vector<size_t> comp_size(comps, 0);
  for (size_t k = 0; k < lbl.size(); ++k)
    if (lbl[k] != Invalid)
      ++comp_size[lbl[k]];
  size_t main_id = 0;
  for (size_t i = 1; i < comp_size.size(); ++i)
    if (comp_size[i] > comp_size[main_id])
      main_id = i;

  // Collect cells of each component.
  std::vector<std::vector<core::Position>> cells(comps);
  for (int y = 0; y < H; ++y)
    for (int x = 0; x < W; ++x) {
      const CompId c = lbl[id(x, y)];
      if (c != Invalid)
        cells[c].push_back({x, y});
    }

  // Connect non-main to main via closest pair.
  for (size_t c = 0; c < comps; ++c) {
    if (c == main_id || cells[c].empty())
      continue;

    int bestD = INT_MAX;
    core::Position A{}, B{};
    for (const auto &p : cells[c])
      for (const auto &q : cells[main_id]) {
        int d = std::abs(p.x - q.x) + std::abs(p.y - q.y);
        if (d < bestD) {
          bestD = d;
          A = p;
          B = q;
        }
      }
    carveLCorridorThroughWalls(m, A, B);

    // Relabel after connecting (simple and robust).
    comps = labelComponents(m, lbl);
    comp_size.assign(comps, 0);
    for (size_t k = 0; k < lbl.size(); ++k)
      if (lbl[k] != Invalid)
        ++comp_size[lbl[k]];
    main_id = 0;
    for (size_t i = 1; i < comp_size.size(); ++i)
      if (comp_size[i] > comp_size[main_id])
        main_id = i;

    cells.assign(comps, {});
    for (int y = 0; y < H; ++y)
      for (int x = 0; x < W; ++x) {
        CompId c2 = lbl[id(x, y)];
        if (c2 != Invalid)
          cells[c2].push_back({x, y});
      }
  }
}

} // namespace

namespace world {

void generateCavesModuleImpl(Map &m, const CavesOptions &opt,
                             const ::config::CaveGenerationConfig &caveGen,
                             std::mt19937 &G) {

  m.fill(Tile::Wall);

  // Seed phase (random interior, solid border).
  std::uniform_int_distribution<int> pct(caveGen.random_percent_min,
                                         caveGen.random_percent_max);
  for (int y = 0; y < m.height(); ++y)
    for (int x = 0; x < m.width(); ++x) {
      const bool edge =
          (x == 0 || y == 0 || x == m.width() - 1 || y == m.height() - 1);
      if (edge)
        m.set({x, y}, Tile::Wall);
      else
        m.set({x, y}, (pct(G) < opt.fill_percent) ? Tile::Wall : Tile::Floor);
    }

  // CA smoothing passes.
  for (int s = 0; s < opt.steps; ++s)
    caStep(m, opt.birth, opt.survive);

  // Ensure connectivity.
  connectComponentsToMain(m);
}

// New overload: accepts LevelConfig
void generateCavesModule(Map &m, const config::LevelConfig &levelCfg,
                         std::mt19937 &G) {
  generateCavesModuleImpl(m, levelCfg.caves, levelCfg.cave_generation, G);
}

// Old overload: backward compatibility wrapper
void generateCavesModule(Map &m, const CavesOptions &opt, std::mt19937 &G) {
  config::LevelConfig cfg;
  cfg.caves = opt;
  generateCavesModule(m, cfg, G);
}
} // namespace world
