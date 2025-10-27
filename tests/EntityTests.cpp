#include "../src/entities/Entity.hpp"
#include "../src/core/Position.hpp"
#include "include/assertions.hpp"
#include <iostream>

int main() {
  using entities::Entity;

  // Test 1: Basic construction
  Entity player("Player", {5, 5});
  EXPECT_EQ(player.getName(), "Player");
  EXPECT_EQ(player.getPosition().x, 5);
  EXPECT_EQ(player.getPosition().y, 5);

  // Test 2: Position updates
  player.setPosition({10, 15});
  EXPECT_EQ(player.getPosition().x, 10);
  EXPECT_EQ(player.getPosition().y, 15);

  // Test 3: Generic properties
  player.setProperty("strength", 18);
  player.setProperty("dexterity", 14);
  EXPECT_EQ(player.getProperty("strength"), 18);
  EXPECT_EQ(player.getProperty("dexterity"), 14);
  EXPECT_TRUE(player.hasProperty("strength"));
  EXPECT_TRUE(!player.hasProperty("intelligence"));

  // Test 4: Default values
  EXPECT_EQ(player.getProperty("missing", 99), 99);

  // Test 5: HP helpers
  player.setMaxHP(100);
  player.setHP(75);
  EXPECT_EQ(player.getMaxHP(), 100);
  EXPECT_EQ(player.getHP(), 75);

  // Test 6: Multiple entities
  Entity goblin("Goblin", {3, 3});
  goblin.setProperty("hp", 20);
  goblin.setProperty("damage", 5);
  EXPECT_EQ(goblin.getName(), "Goblin");
  EXPECT_EQ(goblin.getProperty("hp"), 20);

  std::cout << "Entity tests passed.\n";
  return EXIT_SUCCESS;
}
