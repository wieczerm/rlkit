#include "GameViewPanel.hpp"
#include "core/FOV.hpp"
#include "entities/Entity.hpp"
#include "entities/EntityManager.hpp"
#include "world/Map.hpp"
#include "world/Tile.hpp"

namespace ui {

GameViewPanel::GameViewPanel(int x, int y, int width, int height)
    : bounds_{x, y, width, height}, map_(nullptr), fov_(nullptr),
      entities_(nullptr), cursor_(nullptr) {}

PanelBounds GameViewPanel::getBounds() const { return bounds_; }

void GameViewPanel::setGameState(const world::Map *map, const core::FOV *fov,
                                 const entities::EntityManager *entities,
                                 const core::Position &cameraCenter,
                                 const core::Position *cursor) {
  map_ = map;
  fov_ = fov;
  entities_ = entities;
  cameraCenter_ = cameraCenter;
  cursor_ = cursor;
}

std::string GameViewPanel::render() const {
  if (!map_ || !fov_ || !entities_) {
    size_t size = static_cast<size_t>(bounds_.width) *
                  static_cast<size_t>(bounds_.height);
    return std::string(size, ' ');
  }

  std::string output;

  int startX = cameraCenter_.x - bounds_.width / 2;
  int startY = cameraCenter_.y - bounds_.height / 2;

  for (int y = 0; y < bounds_.height; ++y) {
    for (int x = 0; x < bounds_.width; ++x) {
      int worldX = startX + x;
      int worldY = startY + y;

      if (!map_->inBounds({worldX, worldY}) ||
          !fov_->isVisible(worldX, worldY)) {
        output += ' ';
        continue;
      }

      // Cursor
      if (cursor_ && cursor_->x == worldX && cursor_->y == worldY) {
        output += 'X';
        continue;
      }

      // Entity
      entities::Entity *ent = entities_->getEntityAt({worldX, worldY});
      if (ent) {
        output += ent->getGlyph();
        continue;
      }

      // Tile
      output += tileToChar(static_cast<int>(map_->at({worldX, worldY})));
    }
    output += '\n';
  }

  return output;
}

char GameViewPanel::tileToChar(int tileType) const {
  switch (static_cast<world::Tile>(tileType)) {
  case world::Tile::OpenGround:
    return '.';
  case world::Tile::SolidRock:
    return '#';
    //  case world::Tile::DoorClosed:
    //    return '+';
    //  case world::Tile::DoorOpen:
    //    return '\'';
  default:
    return '?';
  }
}

} // namespace ui
