#include "../src/core/Position.hpp"
#include "../src/entities/Entity.hpp"
#include "../src/entities/EntityManager.hpp"
#include "include/assertions.hpp"
#include <iostream>
#include <memory>

int main() {
  using entities::Entity;
  using entities::EntityManager;

  EntityManager manager;

  // Test 1: Empty manager
  EXPECT_EQ(manager.count(), 0);
  EXPECT_TRUE(manager.getEntityAt({5, 5}) == nullptr);

  // Test 2: Add entities
  auto player = std::make_unique<Entity>("Player", core::Position{5, 5});
  Entity *playerPtr = player.get();
  manager.addEntity(std::move(player));
  EXPECT_EQ(manager.count(), 1);

  auto goblin = std::make_unique<Entity>("Goblin", core::Position{10, 10});
  Entity *goblinPtr = goblin.get();
  manager.addEntity(std::move(goblin));
  EXPECT_EQ(manager.count(), 2);

  // Test 3: Get entity at position
  Entity *found = manager.getEntityAt({5, 5});
  EXPECT_TRUE(found != nullptr);
  EXPECT_EQ(found->getName(), "Player");
  EXPECT_TRUE(found == playerPtr);

  // Test 4: Get entity at empty position
  EXPECT_TRUE(manager.getEntityAt({0, 0}) == nullptr);

  // Test 5: Multiple entities at same position
  auto orc = std::make_unique<Entity>("Orc", core::Position{5, 5});
  manager.addEntity(std::move(orc));
  auto entities = manager.getEntitiesAt({5, 5});
  EXPECT_EQ(entities.size(), 2);

  // Test 6: Remove entity
  manager.removeEntity(goblinPtr);
  EXPECT_EQ(manager.count(), 2);
  EXPECT_TRUE(manager.getEntityAt({10, 10}) == nullptr);

  // Test 7: Clear all
  manager.clear();
  EXPECT_EQ(manager.count(), 0);

  std::cout << "EntityManager tests passed.\n";
  return EXIT_SUCCESS;
}
