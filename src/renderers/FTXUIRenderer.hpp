#pragma once
#include "core/Position.hpp"
#include "world/Tile.hpp"
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <string>
#include <vector>

// Forward declarations
namespace core {
class FOV;
}
namespace world {
class Map;
class FeatureManager;
}
namespace entities {
class EntityManager;
class Entity;
} // namespace entities

namespace renderers {

struct GameState {
  const world::Map *map = nullptr;
  const world::FeatureManager *features = nullptr;
  const core::FOV *fov = nullptr;
  const entities::EntityManager *entities = nullptr;
  const entities::Entity *player = nullptr;
  core::Position cameraCenter;
  const core::Position *cursor = nullptr;

  // UI state
  std::vector<std::string> messages;
  std::string lookInfo;
  bool lookModeActive = false;

  // Stats
  int depth = 1;
  int turn = 0;

  // Minimap
  std::vector<core::Position> discoveredTiles;
};

class FTXUIRenderer {
public:
  FTXUIRenderer(int gameViewWidth = 50, int gameViewHeight = 19);

  // Main render call
  void render(const GameState &state);

private:
  int gameViewWidth_;
  int gameViewHeight_;

  // Build individual panels as FTXUI Elements
  ftxui::Element buildStatusBar(const GameState &state);
  ftxui::Element buildGameView(const GameState &state);
  ftxui::Element buildMessageLog(const GameState &state);
  ftxui::Element buildLookInfo(const GameState &state);
  ftxui::Element buildMinimap(const GameState &state);
  ftxui::Element buildStatsBar(const GameState &state);

  // Helpers
  char tileToChar(world::Tile tile) const;
  std::string buildHPBar(int current, int max, int width) const;
};

} // namespace renderers
