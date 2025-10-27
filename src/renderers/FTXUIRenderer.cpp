#include "src/renderers/FTXUIRenderer.hpp"
#include "../world/Tile.hpp"
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>

namespace renderers {

using namespace ftxui;

FTXUIRenderer::FTXUIRenderer(int width, int height)
    : width_(width), height_(height) {}

char FTXUIRenderer::tileToChar(world::Tile tile) const {
  switch (tile) {
  case world::Tile::Floor:
    return '.';
  case world::Tile::Wall:
    return '#';
  case world::Tile::DoorClosed:
    return '+';
  case world::Tile::DoorOpen:
    return '/';
  default:
    return '?';
  }
}

void FTXUIRenderer::render(const world::Map &map, const FOV &fov,
                           const entities::EntityManager &entityMgr,
                           const Position &cameraCenter) {
  // Calculate camera bounds
  int startX = cameraCenter.x - width_ / 2;
  int startY = cameraCenter.y - height_ / 2;

  // Build map string
  std::string mapStr;
  for (int y = 0; y < height_; ++y) {
    for (int x = 0; x < width_; ++x) {
      int worldX = startX + x;
      int worldY = startY + y;

      // Check if in bounds and visible
      if (!map.inBounds(worldX, worldY) || !fov.isVisible(worldX, worldY)) {
        mapStr += ' ';
        continue;
      }

      // Check for entity at position
      entities::Entity *entity = entityMgr.getEntityAt({worldX, worldY});
      if (entity) {
        mapStr += '@'; // Simple representation for now
      } else {
        mapStr += tileToChar(map.at({worldX, worldY}));
      }
    }
    mapStr += '\n';
  }

  // Simple output for now
  std::cout << mapStr << std::endl;
}

void FTXUIRenderer::renderMessages(const std::vector<std::string> &messages) {
  std::cout << "--- Messages ---" << std::endl;
  for (const auto &msg : messages) {
    std::cout << msg << std::endl;
  }
}

void FTXUIRenderer::renderStats(const std::string &name, int hp, int maxHp) {
  std::cout << "--- Stats ---" << std::endl;
  std::cout << name << " HP: " << hp << "/" << maxHp << std::endl;
}

bool FTXUIRenderer::processFrame() {
  // Simple version - just return true
  // Full version would handle input events
  return true;
}

} // namespace renderers
