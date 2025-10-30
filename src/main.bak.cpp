#include "actions/ActionResult.hpp"
#include "actions/MoveAction.hpp"
#include "actions/OpenAction.hpp"
#include "ai/AIBehavior.hpp"
#include "ai/SimpleAI.hpp"
#include "core/FOV.hpp"
#include "core/InputMapper.hpp"
#include "core/InputScheme.hpp"
#include "core/Pathfinding.hpp"
#include "entities/Entity.hpp"
#include "entities/EntityManager.hpp"
#include "entities/TurnManager.hpp"
#include "renderers/FTXUIRenderer.hpp"
#include "world/Map.hpp"
#include "world/MapViewAdapter.hpp"
#include "world/Tile.hpp"
#include "world/gen/MapGenerator.hpp"
#include "world/gen/RoomsGen.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>

std::string getInfoAt(const core::Position &pos, const world::Map &map,
                      const entities::EntityManager &entityMgr,
                      const core::Position &playerPos) {
  std::stringstream info;

  // Distance
  int dx = std::abs(pos.x - playerPos.x);
  int dy = std::abs(pos.y - playerPos.y);
  int dist = std::max(dx, dy);
  info << "Distance: " << dist << " | ";

  // Tile info
  if (!map.inBounds(pos)) {
    info << "Out of bounds";
    return info.str();
  }

  world::Tile tile = map.at(pos);
  switch (tile) {
  case world::Tile::Floor:
    info << "Floor";
    break;
  case world::Tile::Wall:
    info << "Wall";
    break;
  case world::Tile::DoorClosed:
    info << "Closed door";
    break;
  case world::Tile::DoorOpen:
    info << "Open door";
    break;
  }

  // Entity info
  entities::Entity *entity = entityMgr.getEntityAt(pos);
  if (entity) {
    info << " | " << entity->getName();
    info << " (HP: " << entity->getProperty("hp") << ")";
  }

  return info.str();
}

int main() {
  // Generate map
  constexpr int MAP_W = 60, MAP_H = 40;
  world::Map map(MAP_W, MAP_H, world::Tile::Wall);

  world::MapGenerator gen(12345);
  world::RoomsOptions opts;
  opts.max_rooms = 15;
  opts.room_min = 4;
  opts.room_max = 8;
  opts.add_doors = true;
  gen.generateRooms(map, opts);

  entities::TurnManager turnMgr;

  // Create player
  entities::EntityManager entityMgr;
  auto player =
      std::make_unique<entities::Entity>("Player", core::Position{5, 5});
  player->setMaxHP(100);
  player->setHP(100);
  player->setProperty("str", 10);
  player->setProperty("speed", 100);
  //  std::cout << "Player speed: " << player->getProperty("speed");
  //  getchar();
  entities::Entity *playerPtr = player.get();
  entityMgr.addEntity(std::move(player));
  turnMgr.addEntity(playerPtr); // Add player to turn manager

  // Create some monsters
  std::mt19937 rng(42);
  int monstersSpawned = 0;
  for (int i = 0; i < 5; ++i) {
    int attempts = 0;
    bool spawned = false;
    while (attempts < 100) {
      int x = 1 + (static_cast<int>(rng()) % (MAP_W - 2));
      int y = 1 + (static_cast<int>(rng()) % (MAP_H - 2));
      if (!map.blocksMovement({x, y}) && !entityMgr.getEntityAt({x, y})) {
        auto monster =
            std::make_unique<entities::Entity>("Goblin", core::Position{x, y});
        monster->setMaxHP(30);
        monster->setHP(20);
        monster->setProperty("str", 2);
        monster->setProperty("phys_res", 10);
        monster->setProperty("speed", 60);
        monster->setAI(std::make_unique<ai::SimpleAI>(8));
        entities::Entity *monsterPtr = monster.get();
        entityMgr.addEntity(std::move(monster));
        turnMgr.addEntity(monsterPtr);
        ++monstersSpawned;
        spawned = true;
        break;
      }
      ++attempts;
    }
  }

  // Setup systems
  world::MapViewAdapter mapView(map);
  core::FOV fov(mapView);
  renderers::FTXUIRenderer renderer(40, 25);
  core::InputHandler input;
  core::InputMapper inputMapper(core::Scheme::Vi);

  std::vector<std::string> messages;
  messages.push_back("Welcome to the dungeon!");
  messages.push_back("Use WASD or vi keys to move, Q to quit");

  // Initial FOV compute
  fov.compute(playerPtr->getPosition(), 8);

  bool running = true;

  while (running) {
    // Get input
    char key = input.readChar();
    core::InputAction action = inputMapper.mapInput(key);

    if (action == core::InputAction::Quit) {
      running = false;
      break;
    }

    // Handle player action
    bool playerActed = false;

    if (action == core::InputAction::Open) {
      core::Position target = playerPtr->getPosition();
      if (world::isDoor(map.at({target.x, target.y + 1}))) {
        target.y += 1;
      } else if (world::isDoor(map.at({target.x, target.y - 1}))) {
        target.y -= 1;
      } else if (world::isDoor(map.at({target.x - 1, target.y}))) {
        target.x -= 1;
      } else if (world::isDoor(map.at({target.x + 1, target.y}))) {
        target.x += 1;
      }

      actions::OpenAction open(*playerPtr, target);
      auto result = open.execute(map);
      messages.clear();
      messages.push_back(result.message);

      if (result.status == actions::ActionStatus::Success) {
        playerActed = true;
      }
    }

    else if (action == core::InputAction::Look) {
      // Enter look mode
      core::Position cursor = playerPtr->getPosition();
      bool looking = true;

      while (looking) {
        // Render with cursor
        std::cout << "\033[2J\033[H";
        renderer.render(map, fov, entityMgr, playerPtr->getPosition(), &cursor);
        renderer.renderStats(playerPtr->getName(), playerPtr->getHP(),
                             playerPtr->getMaxHP());

        // Show cursor position marker
        std::cout << "LOOK MODE - Cursor at (" << cursor.x << "," << cursor.y
                  << ")\n";

        // Show info
        std::string info =
            getInfoAt(cursor, map, entityMgr, playerPtr->getPosition());
        std::cout << info << "\n";
        std::cout << "Use WASD to move cursor, X/ESC to exit\n";

        // Get cursor movement
        char lookKey = input.readChar();
        core::InputAction lookAction = inputMapper.mapInput(lookKey);

        if (lookAction == core::InputAction::Look ||
            lookAction == core::InputAction::Quit) {
          looking = false;
          messages.clear();
          messages.push_back("Exited look mode");
        } else if (lookAction != core::InputAction::None &&
                   lookAction != core::InputAction::Wait &&
                   lookAction != core::InputAction::Open) {
          // Move cursor
          core::Position delta =
              core::InputHandler::actionToDirection(lookAction);
          core::Position newCursor = cursor + delta;
          if (map.inBounds(newCursor)) {
            cursor = newCursor;
          }
        }
      }

      continue; // Skip rest of turn processing
    }

    else if (action == core::InputAction::Wait) {
      messages.clear();
      messages.push_back("You wait...");
      playerActed = true;
    } else if (action != core::InputAction::None) {
      // Movement
      core::Position delta = core::InputHandler::actionToDirection(action);
      core::Position newPos = playerPtr->getPosition() + delta;

      actions::MoveAction move(*playerPtr, newPos);
      auto result = move.execute(map, entityMgr, turnMgr);

      messages.clear();
      if (result.status == actions::ActionStatus::Success) {
        messages.push_back(result.message.empty() ? "You move."
                                                  : result.message);
        playerActed = true;
      } else {
        messages.push_back(result.message);
      }
    }

    // If player acted, process turn system
    if (playerActed) {

      // Process AI turns until player's turn again
      int aiTurns = 0;
      while (!turnMgr.isEmpty()) {
        entities::Entity *actor = turnMgr.getNextActor();

        if (actor == playerPtr) {
          turnMgr.processTurn();
          break; // Back to player
        }

        // AI turn
        if (actor->hasAI()) {
          auto result =
              actor->getAI()->act(*actor, *playerPtr, map, entityMgr, turnMgr);

          if (!result.message.empty()) {
            messages.push_back(result.message);
          }
        } else {
        }

        turnMgr.processTurn(); // Consume AI turn
      }

      // Update FOV after all actions
      fov.compute(playerPtr->getPosition(), 8);
    }

    // Render
    std::cout << "\033[2J\033[H"; // Clear screen
    renderer.render(map, fov, entityMgr, playerPtr->getPosition());
    renderer.renderStats(playerPtr->getName(), playerPtr->getHP(),
                         playerPtr->getMaxHP());
    renderer.renderMessages(messages);
  }

  std::cout << "\nThanks for playing!\n";
  return 0;
}
