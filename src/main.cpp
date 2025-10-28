#include "core/FOV.hpp"
#include "core/InputHandler.hpp"
#include "core/Pathfinding.hpp"
#include "entities/Entity.hpp"
#include "entities/EntityManager.hpp"
#include "entities/TurnManager.hpp"
#include "renderers/FTXUIRenderer.hpp"
#include "src/actions/ActionResult.hpp"
#include "src/actions/MoveAction.hpp"
#include "src/actions/OpenAction.hpp"
#include "world/Map.hpp"
#include "world/MapViewAdapter.hpp"
#include "world/Tile.hpp"
#include "world/gen/MapGenerator.hpp"
#include "world/gen/RoomsGen.hpp"
#include <iostream>
#include <memory>
#include <random>

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

  // Create player
  entities::EntityManager entityMgr;
  auto player = std::make_unique<entities::Entity>("Player", Position{5, 5});
  player->setMaxHP(100);
  player->setHP(100);
  player->setProperty("speed", 100);
  entities::Entity *playerPtr = player.get();
  entityMgr.addEntity(std::move(player));

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
            std::make_unique<entities::Entity>("Goblin", Position{x, y});
        monster->setMaxHP(20);
        monster->setHP(20);
        monster->setProperty("speed", 80);
        entityMgr.addEntity(std::move(monster));
        ++monstersSpawned;
        spawned = true;
        break;
      }
      ++attempts;
    }
  }

  // Setup systems
  world::MapViewAdapter mapView(map);
  FOV fov(mapView);
  renderers::FTXUIRenderer renderer(40, 25);
  core::InputHandler input;

  std::vector<std::string> messages;
  messages.push_back("Welcome to the dungeon!");
  messages.push_back("Use WASD or vi keys to move, Q to quit");

  // Game loop
  bool running = true;
  while (running) {

    core::InputAction action = input.getAction();

    if (action == core::InputAction::Open) {
      Position target = playerPtr->getPosition();
      if (world::isDoor(map.at({target.x, target.y + 1})))
        target.y += 1; // south
      if (world::isDoor(map.at({target.x, target.y - 1})))
        target.y -= 1; // north
      if (world::isDoor(map.at({target.x - 1, target.y})))
        target.x -= 1; // west
      if (world::isDoor(map.at({target.x + 1, target.y})))
        target.x += 1; // east
      actions::OpenAction open(*playerPtr, target);
      auto result = open.execute(map);
      messages.clear();
      messages.push_back(result.message);
    } else if (action == core::InputAction::Quit) {
      running = false;
    } else if (action != core::InputAction::None &&
               action != core::InputAction::Wait) {
      // Movement actions
      Position delta = core::InputHandler::actionToDirection(action);
      Position newPos = playerPtr->getPosition() + delta;

      actions::MoveAction move(*playerPtr, newPos);
      auto result = move.execute(map, entityMgr);

      messages.clear();
      if (result.status == actions::ActionStatus::Success) {
        if (!result.message.empty()) {
          messages.push_back(result.message);
        } else {
          messages.push_back("You move.");
        }
      } else {
        messages.push_back(result.message);
      }
    }
    // Update FOV
    fov.compute(playerPtr->getPosition(), 8);

    // Render
    std::cout << "\033[2J\033[H"; // Clear screen
    renderer.render(map, fov, entityMgr, playerPtr->getPosition());
    renderer.renderStats(playerPtr->getName(), playerPtr->getHP(),
                         playerPtr->getMaxHP());
    renderer.renderMessages(messages);

    // Get input
    // core::InputAction action = input.getAction();

    if (action == core::InputAction::Quit) {
      running = false;
      continue;
    }

    if (action == core::InputAction::Wait) {
      messages.clear();
      messages.push_back("You wait...");
      continue;
    }

    // Handle movement
    /* Position delta = core::InputHandler::actionToDirection(action);
     if (delta.x != 0 || delta.y != 0) {
       Position newPos = playerPtr->getPosition() + delta;

       if (map.inBounds(newPos) && !map.blocksMovement(newPos)) {
         // Check for entity collision
         entities::Entity *target = entityMgr.getEntityAt(newPos);
         if (target) {
           messages.clear();
           messages.push_back("You bump into " + target->getName());
         } else {
           playerPtr->setPosition(newPos);
           messages.clear();
           messages.push_back("You move.");
         }
       } else {
         messages.clear();
         messages.push_back("Blocked!");
       }
     }*/
  }

  std::cout << "\nThanks for playing!\n";
  return 0;
}
