#pragma once
#include "IMapView.hpp"
#include "Position.hpp"
#include <vector>

class FOV {
public:
  explicit FOV(const core::IMapView &map);

  void compute(const Position &origin, int radius);
  bool isVisible(int x, int y) const;

private:
  const core::IMapView &map_;
  size_t width_;
  size_t height_;
  std::vector<std::vector<bool>> visible_;

  static const int multXX[8];
  static const int multXY[8];
  static const int multYX[8];
  static const int multYY[8];

  void castLight(int cx, int cy, int row, double startSlope, double endSlope,
                 int radius, int xx, int xy, int yx, int yy);
};
