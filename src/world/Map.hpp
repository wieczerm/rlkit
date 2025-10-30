#pragma once
#include "Tile.hpp"
#include "core/Position.hpp"
#include <cassert>
#include <cstddef>
#include <vector>

namespace world {

class Map {
public:
  Map(int w, int h, Tile fill = Tile::Wall)
      : w_(w), h_(h),
        data_(static_cast<std::size_t>(w) * static_cast<std::size_t>(h), fill) {
    assert(w_ > 0 && h_ > 0);
  }

  int width() const noexcept { return w_; }
  int height() const noexcept { return h_; }

  [[nodiscard]] inline bool inBounds(core::Position p) const noexcept {
    return p.x >= 0 && p.y >= 0 && p.x < w_ && p.y < h_;
  }

  [[nodiscard]] inline bool inBounds(int x, int y) const noexcept {
    return x >= 0 && y >= 0 && x < w_ && y < h_;
  }

  Tile at(core::Position p) const noexcept {
    assert(inBounds(p));
    return data_[idx(p)];
  }

  void set(core::Position p, Tile t) noexcept {
    assert(inBounds(p));
    data_[idx(p)] = t;
  }

  void fill(Tile t) noexcept {
    for (int y = 0; y < height(); ++y)
      for (int x = 0; x < width(); ++x)
        set({x, y}, t);
  }

  inline bool isOpaque(int x, int y) const noexcept {
    if (x < 0 || x >= w_ || y < 0 || y >= h_) {
      return true; // poza mapą traktujemy jako blokadę
    }
    return ::world::blocksLineOfSight(data_[idx({x, y})]);
  }

  bool blocksMovement(core::Position p) const noexcept {
    if (!inBounds(p))
      return true;
    return ::world::blocksMovement(at(p));
  }

  inline bool blocksLineOfSight(core::Position p) const noexcept {
    return ::world::blocksLineOfSight(at(p));
  }

private:
  std::size_t idx(core::Position p) const noexcept {
    // bezpieczne po in_bounds()
    return static_cast<std::size_t>(p.y) * static_cast<std::size_t>(w_) +
           static_cast<std::size_t>(p.x);
  }

  int w_;
  int h_;
  std::vector<Tile> data_;
};

} // namespace world
