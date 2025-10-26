#pragma once
#include "Position.hpp"

namespace core {
struct IMapView {
  virtual ~IMapView() = default;
  virtual int width() const noexcept = 0;
  virtual int height() const noexcept = 0;
  // True jeśli BLOKUJE LOS:
  virtual bool blocksLos(int x, int y) const noexcept = 0;
};
} // namespace core
