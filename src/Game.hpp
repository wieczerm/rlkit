#pragma once
#include "core/InputMapper.hpp"
#include "entities/TurnManager.hpp"
#include "renderers/FTXUIRenderer.hpp"
#include "world/Map.hpp"
#include "world/MapViewAdapter.hpp"
#include <memory>
#include <string>
#include <vector>

// Forward declarations as before...

class Game {
public:
  Game();
  ~Game();

  void run();

private:
  void handleInput();
  void handleLookMode();
  void descendStairs();
  void generateLevel();
  void processPlayerTurn();
  void processAITurns();
  void render();
  void addMessage(const std::string &msg);

  std::string getInfoAt(const core::Position &pos,
                        const core::Position &playerPos) const;

  // Game state
  std::unique_ptr<world::Map> map_;
  std::unique_ptr<world::MapViewAdapter> mapView_;
  std::unique_ptr<core::FOV> fov_;
  std::unique_ptr<entities::EntityManager> entityMgr_;
  std::unique_ptr<entities::TurnManager> turnMgr_;
  std::unique_ptr<core::InputMapper> inputMapper_;

  // Renderer (simplified!)
  std::unique_ptr<renderers::FTXUIRenderer> renderer_;

  // UI state
  std::vector<std::string> messages_;
  std::vector<core::Position> discoveredTiles_;
  std::string lookInfo_;
  bool lookModeActive_;
  core::Position lookCursor_;

  entities::Entity *playerPtr_;
  bool running_;
  int turnCounter_;
  int depth_;
};
