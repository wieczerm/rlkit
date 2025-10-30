#pragma once
#include "Panel.hpp"
#include "core/Position.hpp"
#include <set>
#include <vector>

namespace ui {

class MinimapPanel : public Panel {
public:
  MinimapPanel(int x, int y, int width, int height, int mapWidth,
               int mapHeight);

  std::string render() const override;
  PanelBounds getBounds() const override;

  // Mark tile as discovered
  void discover(const core::Position &pos);

  // Update player position
  void setPlayerPosition(const core::Position &pos) { playerPos_ = pos; }

  // Clear all discovered tiles
  void clear();

private:
  PanelBounds bounds_;
  int mapWidth_;
  int mapHeight_;
  std::set<std::pair<int, int>> discoveredTiles_;
  core::Position playerPos_;

  // Scale map to fit minimap
  int scaleX_;
  int scaleY_;
};

} // namespace ui
