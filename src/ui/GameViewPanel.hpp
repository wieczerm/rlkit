#pragma once
#include "Panel.hpp"
#include "core/Position.hpp"
#include <memory>

// Forward declarations
namespace core {
class FOV;
}
namespace world {
class Map;
}
namespace entities {
class EntityManager;
}

namespace ui {

class GameViewPanel : public Panel {
public:
  GameViewPanel(int x, int y, int width, int height);

  std::string render() const override;
  PanelBounds getBounds() const override;

  // Update what we're rendering
  void setGameState(const world::Map *map, const core::FOV *fov,
                    const entities::EntityManager *entities,
                    const core::Position &cameraCenter,
                    const core::Position *cursor = nullptr);

private:
  PanelBounds bounds_;

  // Non-owning pointers - just for rendering
  const world::Map *map_;
  const core::FOV *fov_;
  const entities::EntityManager *entities_;
  core::Position cameraCenter_;
  const core::Position *cursor_;

  char tileToChar(int tileType) const;
};

} // namespace ui
