#include "Game.hpp"

// Remove old UI includes, keep only:
#include "renderers/FTXUIRenderer.hpp"

// Keep other includes
#include "actions/MoveAction.hpp"
#include "actions/OpenAction.hpp"
#include "ai/SimpleAI.hpp"
#include "core/FOV.hpp"
#include "core/InputAction.hpp"
#include "core/InputHandler.hpp"
#include "core/InputMapper.hpp"
#include "core/InputScheme.hpp"
#include "entities/Entity.hpp"
#include "entities/EntityManager.hpp"
#include "entities/TurnManager.hpp"
#include "world/FeatureProperties.hpp"
#include "world/Map.hpp"
#include "world/MapViewAdapter.hpp"
#include "world/Tile.hpp"
#include "world/gen/LevelGenerator.hpp"

#include <fstream>
#include <iostream>
#include <random>
#include <sstream>

Game::Game()
    : playerPtr_(nullptr), running_(true), turnCounter_(0), depth_(1),
      lookModeActive_(false), lookCursor_{0, 0} {

  inputMapper_ = std::make_unique<core::InputMapper>(core::Scheme::Vi);
  renderer_ = std::make_unique<renderers::FTXUIRenderer>(50, 19);

  generateLevel(); // TYLKO TO

  addMessage("Welcome to the dungeon!");
  addMessage("Use hjkl/WASD to move, x to look, > to descend, q to quit");
}

Game::~Game() = default;

void Game::run() {
  while (running_) {
    render();
    handleInput();
  }

  std::cout << "\nThanks for playing!\n";
}

void Game::render() {
  // Build game state
  renderers::GameState state;
  state.map = map_.get();
  state.features = featureMgr_.get();
  state.fov = fov_.get();
  state.entities = entityMgr_.get();
  state.player = playerPtr_;
  state.cameraCenter = playerPtr_->getPosition();
  state.cursor = lookModeActive_ ? &lookCursor_ : nullptr;

  state.messages = messages_;
  state.lookInfo = lookInfo_;
  state.lookModeActive = lookModeActive_;

  state.depth = depth_;
  state.turn = turnCounter_;

  state.discoveredTiles = discoveredTiles_;

  // Render!
  renderer_->render(state);
}

void Game::handleInput() {
  core::InputHandler input;
  char key = input.readChar();
  core::InputAction action = inputMapper_->mapInput(key);

  if (action == core::InputAction::Quit) {
    running_ = false;
    return;
  }

  if (action == core::InputAction::Look) {
    handleLookMode();
    return;
  }

  if (action == core::InputAction::Descend) { // NEW
    descendStairs();
    return;
  }

  bool playerActed = false;

  // Handle player actions
  if (action == core::InputAction::Open) {
    core::Position target = playerPtr_->getPosition();

    // Check adjacent positions for doors (via Features)
    auto checkDoor = [this](core::Position pos) -> bool {
      if (!map_->inBounds(pos))
        return false;
      const world::Feature *f = featureMgr_->getFeature(pos);
      return f && world::isDoor(*f);
    };

    if (checkDoor({target.x, target.y + 1})) {
      target.y += 1;
    } else if (checkDoor({target.x, target.y - 1})) {
      target.y -= 1;
    } else if (checkDoor({target.x - 1, target.y})) {
      target.x -= 1;
    } else if (checkDoor({target.x + 1, target.y})) {
      target.x += 1;
    }

    actions::OpenAction open(*playerPtr_, target);
    auto result = open.execute(*map_, *featureMgr_);
    addMessage(result.message);

    if (result.status == actions::ActionStatus::Success) {
      playerActed = true;
    }
  } else if (action == core::InputAction::Wait) {
    addMessage("You wait...");
    playerActed = true;
  } else if (action != core::InputAction::None) {
    // Movement
    core::Position delta = core::InputHandler::actionToDirection(action);
    core::Position newPos = playerPtr_->getPosition() + delta;

    actions::MoveAction move(*playerPtr_, newPos);
    auto result = move.execute(*map_, *featureMgr_, *entityMgr_, *turnMgr_);

    if (result.status == actions::ActionStatus::Success) {
      if (!result.message.empty()) {
        addMessage(result.message);
      } else {
        // messages_.push_back("You move.");
      }
      playerActed = true;
    } else {
      addMessage(result.message);
    }
  }

  if (playerActed) {
    turnCounter_++;
    processPlayerTurn();
    processAITurns();
    fov_->compute(playerPtr_->getPosition(), 8);

    // Discover newly visible tiles
    const int MAP_W = 60, MAP_H = 40;
    for (int y = 0; y < MAP_H; ++y) {
      for (int x = 0; x < MAP_W; ++x) {
        if (fov_->isVisible(x, y)) {
          // Check if already discovered
          bool alreadyDiscovered = false;
          for (const auto &tile : discoveredTiles_) {
            if (tile.x == x && tile.y == y) {
              alreadyDiscovered = true;
              break;
            }
          }
          if (!alreadyDiscovered) {
            discoveredTiles_.push_back({x, y});
          }
        }
      }
    }
  }
}

void Game::generateLevel() {
  constexpr int MAP_W = 60, MAP_H = 40;

  // Preserve HP BEFORE destroying entities
  int preservedHP = 100; // Default for first level
  if (playerPtr_ != nullptr) {
    preservedHP = playerPtr_->getProperty("hp");
  }

  // Clear existing entities
  entityMgr_ = std::make_unique<entities::EntityManager>();
  turnMgr_ = std::make_unique<entities::TurnManager>();

  // Generate level using new LevelGenerator
  std::mt19937 rng(std::random_device{}());
  world::LevelGenerator generator(rng);
  world::LevelData levelData = generator.generateLevel(MAP_W, MAP_H, depth_);

  // Take ownership of generated map and features
  map_ = std::move(levelData.map);
  featureMgr_ = std::move(levelData.features);

  // DEBUG - count stairs via features
  int stairsCount = 0;
  for (const auto &pos : featureMgr_->getAllPositions()) {
    const world::Feature *f = featureMgr_->getFeature(pos);
    if (f && world::isStairs(*f)) {
      stairsCount++;
    }
  }
  addMessage("Generated level with " + std::to_string(stairsCount) + " stairs");
  // end DEBUG

  // Use generated spawn position
  core::Position spawnPos = levelData.player_spawn;

  // Recreate player at spawn
  auto player = std::make_unique<entities::Entity>("Player", spawnPos);
  player->setMaxHP(100);
  player->setHP(preservedHP); // Use preserved value
  player->setProperty("speed", 100);
  player->setProperty("str", 10);
  player->setGlyph('@');
  playerPtr_ = player.get();
  entityMgr_->addEntity(std::move(player));
  turnMgr_->addEntity(playerPtr_);

  // Spawn monsters at generated positions
  for (const auto &monsterPos : levelData.monster_spawns) {
    auto monster = std::make_unique<entities::Entity>("Goblin", monsterPos);
    monster->setMaxHP(30);
    monster->setHP(20);
    monster->setProperty("str", 2);
    monster->setProperty("phys_res", 2);
    monster->setProperty("speed", 100);
    monster->setGlyph('g');
    monster->setAI(std::make_unique<ai::SimpleAI>(8));
    entities::Entity *monsterPtr = monster.get();
    entityMgr_->addEntity(std::move(monster));
    turnMgr_->addEntity(monsterPtr);
  }

  // Reset FOV and discovered tiles
  mapView_ = std::make_unique<world::MapViewAdapter>(*map_);
  fov_ = std::make_unique<core::FOV>(*mapView_);
  fov_->compute(playerPtr_->getPosition(), 8);

  discoveredTiles_.clear();
  for (int y = 0; y < MAP_H; ++y) {
    for (int x = 0; x < MAP_W; ++x) {
      if (fov_->isVisible(x, y)) {
        discoveredTiles_.push_back({x, y});
      }
    }
  }
}

void Game::descendStairs() {
  std::cout << "descendStairs() - start\n";
  std::cout.flush();

  // Save state
  int currentHP = playerPtr_->getProperty("hp");
  std::cout << "HP saved: " << currentHP << "\n";
  std::cout.flush();

  core::Position playerPos = playerPtr_->getPosition();
  std::cout << "Position: " << playerPos.x << "," << playerPos.y << "\n";
  std::cout.flush();

  // Check if there's a stairs feature at player position
  const world::Feature *feature = featureMgr_->getFeature(playerPos);
  bool hasStairs = feature && world::isStairs(*feature);

  std::cout << "Has stairs feature: " << hasStairs << "\n";
  std::cout.flush();

  if (!hasStairs) {
    std::cout << "Not on stairs\n";
    addMessage("There are no stairs here.");
    return;
  }

  std::cout << "Descending...\n";
  std::cout.flush();

  depth_++;
  addMessage("You descend the stairs...");

  std::cout << "Calling generateLevel()...\n";
  std::cout.flush();

  generateLevel();

  std::cout << "Level generated, restoring HP\n";
  std::cout.flush();

  playerPtr_->setProperty("hp", currentHP);

  std::cout << "Done!\n";
  std::cout.flush();

  addMessage("Welcome to depth " + std::to_string(depth_) + "!");
}

void Game::handleLookMode() {
  lookCursor_ = playerPtr_->getPosition();
  lookModeActive_ = true;

  bool looking = true;
  core::InputHandler input;

  while (looking) {
    // Update look info
    lookInfo_ = getInfoAt(lookCursor_, playerPtr_->getPosition());

    // Render
    render();

    // Get input
    char lookKey = input.readChar();
    core::InputAction lookAction = inputMapper_->mapInput(lookKey);

    if (lookAction == core::InputAction::Look ||
        lookAction == core::InputAction::Quit) {
      looking = false;
      lookModeActive_ = false;
      lookInfo_.clear();
      addMessage("Exited look mode");
    } else if (lookAction != core::InputAction::None &&
               lookAction != core::InputAction::Wait &&
               lookAction != core::InputAction::Open) {
      core::Position delta = core::InputHandler::actionToDirection(lookAction);
      core::Position newCursor = lookCursor_ + delta;
      if (map_->inBounds(newCursor) &&
          fov_->isVisible(newCursor.x, newCursor.y)) {
        lookCursor_ = newCursor;
      }
    }
  }
}

void Game::processPlayerTurn() { turnMgr_->processTurn(); }

void Game::processAITurns() {
  while (!turnMgr_->isEmpty()) {
    entities::Entity *actor = turnMgr_->getNextActor();

    if (actor == playerPtr_) {
      break;
    }

    if (actor->hasAI()) {
      auto result = actor->getAI()->act(*actor, *playerPtr_, *map_,
                                        *featureMgr_, *entityMgr_, *turnMgr_);
      if (!result.message.empty()) {
        addMessage(result.message);
      }
    }

    turnMgr_->processTurn();
  }
}

std::string Game::getInfoAt(const core::Position &pos,
                            const core::Position &playerPos) const {
  std::stringstream info;

  int dx = std::abs(pos.x - playerPos.x);
  int dy = std::abs(pos.y - playerPos.y);
  int dist = std::max(dx, dy);
  info << "Dist: " << dist << " | ";

  if (!map_->inBounds(pos)) {
    info << "Out of bounds";
    return info.str();
  }

  // Check for features first (stairs, doors)
  const world::Feature *feature = featureMgr_->getFeature(pos);
  if (feature) {
    if (world::isStairs(*feature)) {
      const world::Stairs *stairs = world::getStairs(*feature);
      info << (stairs->direction == world::Stairs::Direction::Down
                   ? "Stairs down"
                   : "Stairs up");
    } else if (world::isDoor(*feature)) {
      const world::Door *door = world::getDoor(*feature);
      info << (door->state == world::Door::State::Closed ? "Closed door"
                                                         : "Open door");
    }
  } else {
    // No feature, show tile
    world::Tile tile = map_->at(pos);
    switch (tile) {
    case world::Tile::OpenGround:
      info << "Floor";
      break;
    case world::Tile::SolidRock:
      info << "Wall";
      break;
      /*    case world::Tile::DoorClosed:
            info << "Closed door (legacy)";
            break;
          case world::Tile::DoorOpen:
            info << "Open door (legacy)";
            break;*/
    default:
      info << "Unknown";
      break;
    }
  }

  entities::Entity *entity = entityMgr_->getEntityAt(pos);
  if (entity) {
    info << " | " << entity->getName();
    info << " (HP:" << entity->getProperty("hp") << ")";
  }

  return info.str();
}

void Game::addMessage(const std::string &msg) {
  messages_.push_back(msg);

  // Keep last 1000 messages
  if (messages_.size() > 1000) {
    messages_.erase(messages_.begin());
  }
}
