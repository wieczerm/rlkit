#include "MinimapPanel.hpp"
#include <algorithm>

namespace ui {

MinimapPanel::MinimapPanel(int x, int y, int width, int height, int mapWidth,
                           int mapHeight)
    : bounds_{x, y, width, height}, mapWidth_(mapWidth), mapHeight_(mapHeight),
      playerPos_{0, 0} {

  // Calculate scaling
  scaleX_ = std::max(1, mapWidth / (width - 2));
  scaleY_ = std::max(1, mapHeight / (height - 3)); // Account for title
}

PanelBounds MinimapPanel::getBounds() const { return bounds_; }

void MinimapPanel::discover(const core::Position &pos) {
  discoveredTiles_.insert({pos.x, pos.y});
}

void MinimapPanel::clear() { discoveredTiles_.clear(); }

std::string MinimapPanel::render() const {
  std::string output;

  // Title
  output += "Minimap:\n";
  output += std::string(bounds_.width, '-') + "\n";

  int displayHeight = bounds_.height - 2;
  int displayWidth = bounds_.width;

  for (int y = 0; y < displayHeight; ++y) {
    for (int x = 0; x < displayWidth; ++x) {
      // Map display coordinates to world coordinates
      int worldX = x * scaleX_;
      int worldY = y * scaleY_;

      // Check if player is in this cell
      if (playerPos_.x / scaleX_ == x && playerPos_.y / scaleY_ == y) {
        output += '@';
        continue;
      }

      // Check if any tile in this scaled region is discovered
      bool discovered = false;
      for (int dx = 0; dx < scaleX_ && !discovered; ++dx) {
        for (int dy = 0; dy < scaleY_ && !discovered; ++dy) {
          if (discoveredTiles_.count({worldX + dx, worldY + dy})) {
            discovered = true;
          }
        }
      }

      output += discovered ? '.' : ' ';
    }
    output += '\n';
  }

  return output;
}

} // namespace ui
