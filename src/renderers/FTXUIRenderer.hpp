#pragma once
#include "../world/Map.hpp"
#include "../entities/EntityManager.hpp"
#include "../core/FOV.hpp"
#include <string>

namespace renderers {

class FTXUIRenderer {
public:
  FTXUIRenderer(int width, int height);

  // Render map with FOV and entities
  void render(const world::Map &map, const FOV &fov, 
              const entities::EntityManager &entityMgr,
              const Position &cameraCenter);

  // Render a message log area
  void renderMessages(const std::vector<std::string> &messages);

  // Render stats panel
  void renderStats(const std::string &name, int hp, int maxHp);

  // Run the main render loop (returns false when should exit)
  bool processFrame();

private:
  int width_;
  int height_;

  // Convert tile to character
  char tileToChar(world::Tile tile) const;
};

} // namespace renderers
