#include "../src/entities/TurnManager.hpp"
#include "../src/entities/Entity.hpp"
#include "../src/core/Position.hpp"
#include "include/assertions.hpp"
#include <iostream>

int main() {
  using entities::TurnManager;
  using entities::Entity;

  TurnManager turnMgr;

  // Test 1: Empty manager
  EXPECT_TRUE(turnMgr.isEmpty());
  EXPECT_TRUE(turnMgr.getNextActor() == nullptr);

  // Test 2: Add entities with different speeds
  Entity player("Player", {0, 0});
  player.setProperty("speed", 100);

  Entity fastGoblin("FastGoblin", {1, 1});
  fastGoblin.setProperty("speed", 150);

  Entity slowOrc("SlowOrc", {2, 2});
  slowOrc.setProperty("speed", 50);

  turnMgr.addEntity(&player);
  turnMgr.addEntity(&fastGoblin);
  turnMgr.addEntity(&slowOrc);
  EXPECT_TRUE(!turnMgr.isEmpty());

  // Test 3: Fastest entity acts first
  Entity *actor = turnMgr.getNextActor();
  EXPECT_TRUE(actor != nullptr);
  EXPECT_EQ(actor->getName(), "FastGoblin");

  // Test 4: Process turns - fast entity should act more often
  actor = turnMgr.processTurn();
  EXPECT_EQ(actor->getName(), "FastGoblin");

  actor = turnMgr.processTurn();
  EXPECT_EQ(actor->getName(), "Player");

  // Test 5: Remove entity
  turnMgr.removeEntity(&slowOrc);
  for (int i = 0; i < 10; ++i) {
    actor = turnMgr.processTurn();
    EXPECT_TRUE(actor->getName() != "SlowOrc");
  }

  // Test 6: Clear all
  turnMgr.clear();
  EXPECT_TRUE(turnMgr.isEmpty());

  std::cout << "TurnManager tests passed.\n";
  return EXIT_SUCCESS;
}
