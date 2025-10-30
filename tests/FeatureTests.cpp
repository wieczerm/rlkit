#include "../src/world/Feature.hpp"
#include "../src/world/FeatureManager.hpp"
#include "../src/world/FeatureProperties.hpp"
#include "assertions.hpp"
#include <iostream>

using namespace world;
using namespace core;

// Test Door creation and states
void testDoorStates() {
  std::cout << "Testing Door states..." << std::endl;

  Door open_door{Door::Material::Wood, Door::State::Open};
  Door closed_door{Door::Material::Iron, Door::State::Closed};

  EXPECT_TRUE(open_door.state == Door::State::Open);
  EXPECT_TRUE(closed_door.state == Door::State::Closed);
  EXPECT_TRUE(open_door.material == Door::Material::Wood);
  EXPECT_TRUE(closed_door.material == Door::Material::Iron);

  std::cout << "  ✓ Door states work correctly" << std::endl;
}

// Test Stairs creation and directions
void testStairsDirections() {
  std::cout << "Testing Stairs directions..." << std::endl;

  Stairs down_stairs{Stairs::Direction::Down, 2};
  Stairs up_stairs{Stairs::Direction::Up, 1};

  EXPECT_TRUE(down_stairs.direction == Stairs::Direction::Down);
  EXPECT_TRUE(up_stairs.direction == Stairs::Direction::Up);
  EXPECT_EQ(down_stairs.target_depth, 2);
  EXPECT_EQ(up_stairs.target_depth, 1);

  std::cout << "  ✓ Stairs directions work correctly" << std::endl;
}

// Test feature variant
void testFeatureVariant() {
  std::cout << "Testing Feature variant..." << std::endl;

  Feature door_feature = Door{Door::Material::Wood, Door::State::Closed};
  Feature stairs_feature = Stairs{Stairs::Direction::Down, 5};

  EXPECT_TRUE(std::holds_alternative<Door>(door_feature));
  EXPECT_TRUE(std::holds_alternative<Stairs>(stairs_feature));
  EXPECT_FALSE(std::holds_alternative<Stairs>(door_feature));
  EXPECT_FALSE(std::holds_alternative<Door>(stairs_feature));

  std::cout << "  ✓ Feature variant holds correct types" << std::endl;
}

// Test blocksMovement property
void testBlocksMovement() {
  std::cout << "Testing blocksMovement property..." << std::endl;

  Feature open_door = Door{Door::Material::Wood, Door::State::Open};
  Feature closed_door = Door{Door::Material::Wood, Door::State::Closed};
  Feature stairs = Stairs{Stairs::Direction::Down, 2};

  EXPECT_FALSE(blocksMovement(open_door));  // Open door doesn't block
  EXPECT_TRUE(blocksMovement(closed_door)); // Closed door blocks
  EXPECT_FALSE(blocksMovement(stairs));     // Stairs don't block

  std::cout << "  ✓ blocksMovement works correctly" << std::endl;
}

// Test blocksLineOfSight property
void testBlocksLineOfSight() {
  std::cout << "Testing blocksLineOfSight property..." << std::endl;

  Feature open_door = Door{Door::Material::Wood, Door::State::Open};
  Feature closed_door = Door{Door::Material::Wood, Door::State::Closed};
  Feature stairs = Stairs{Stairs::Direction::Down, 2};

  EXPECT_FALSE(blocksLineOfSight(open_door));  // Open door doesn't block LOS
  EXPECT_TRUE(blocksLineOfSight(closed_door)); // Closed door blocks LOS
  EXPECT_FALSE(blocksLineOfSight(stairs));     // Stairs don't block LOS

  std::cout << "  ✓ blocksLineOfSight works correctly" << std::endl;
}

// Test isInteractable property
void testIsInteractable() {
  std::cout << "Testing isInteractable property..." << std::endl;

  Feature door = Door{Door::Material::Wood, Door::State::Closed};
  Feature stairs = Stairs{Stairs::Direction::Down, 2};

  EXPECT_TRUE(isInteractable(door));
  EXPECT_TRUE(isInteractable(stairs));

  std::cout << "  ✓ isInteractable works correctly" << std::endl;
}

// Test getGlyph property
void testGetGlyph() {
  std::cout << "Testing getGlyph property..." << std::endl;

  Feature open_door = Door{Door::Material::Wood, Door::State::Open};
  Feature closed_door = Door{Door::Material::Wood, Door::State::Closed};
  Feature down_stairs = Stairs{Stairs::Direction::Down, 2};
  Feature up_stairs = Stairs{Stairs::Direction::Up, 1};

  EXPECT_EQ(getGlyph(open_door), '\'');
  EXPECT_EQ(getGlyph(closed_door), '+');
  EXPECT_EQ(getGlyph(down_stairs), '>');
  EXPECT_EQ(getGlyph(up_stairs), '<');

  std::cout << "  ✓ getGlyph returns correct glyphs" << std::endl;
}

// Test type checking helpers
void testTypeCheckers() {
  std::cout << "Testing type checker helpers..." << std::endl;

  Feature door = Door{Door::Material::Wood, Door::State::Closed};
  Feature stairs = Stairs{Stairs::Direction::Down, 2};

  EXPECT_TRUE(isDoor(door));
  EXPECT_FALSE(isStairs(door));

  EXPECT_TRUE(isStairs(stairs));
  EXPECT_FALSE(isDoor(stairs));

  std::cout << "  ✓ Type checkers work correctly" << std::endl;
}

// Test getDoor/getStairs helpers
void testGetHelpers() {
  std::cout << "Testing getDoor/getStairs helpers..." << std::endl;

  Feature door_feature = Door{Door::Material::Iron, Door::State::Closed};
  Feature stairs_feature = Stairs{Stairs::Direction::Up, 3};

  Door *door = getDoor(door_feature);
  const Stairs *stairs = getStairs(stairs_feature);

  EXPECT_TRUE(door != nullptr);
  EXPECT_TRUE(stairs != nullptr);
  EXPECT_TRUE(door->material == Door::Material::Iron);
  EXPECT_TRUE(stairs->direction == Stairs::Direction::Up);

  // Cross-type checks should return nullptr
  EXPECT_TRUE(getStairs(door_feature) == nullptr);
  EXPECT_TRUE(getDoor(stairs_feature) == nullptr);

  std::cout << "  ✓ Get helpers work correctly" << std::endl;
}

// Test FeatureManager add/remove
void testFeatureManagerAddRemove() {
  std::cout << "Testing FeatureManager add/remove..." << std::endl;

  FeatureManager manager;
  Position pos{5, 10};

  EXPECT_FALSE(manager.hasFeature(pos));
  EXPECT_EQ(manager.size(), 0u);

  manager.addFeature(pos, Door{Door::Material::Wood, Door::State::Closed});

  EXPECT_TRUE(manager.hasFeature(pos));
  EXPECT_EQ(manager.size(), 1u);

  manager.removeFeature(pos);

  EXPECT_FALSE(manager.hasFeature(pos));
  EXPECT_EQ(manager.size(), 0u);

  std::cout << "  ✓ Add/remove work correctly" << std::endl;
}

// Test FeatureManager getFeature
void testFeatureManagerGet() {
  std::cout << "Testing FeatureManager getFeature..." << std::endl;

  FeatureManager manager;
  Position door_pos{3, 7};
  Position empty_pos{10, 10};

  manager.addFeature(door_pos, Door{Door::Material::Stone, Door::State::Open});

  Feature *feature = manager.getFeature(door_pos);
  EXPECT_TRUE(feature != nullptr);
  EXPECT_TRUE(isDoor(*feature));

  Door *door = getDoor(*feature);
  EXPECT_TRUE(door->material == Door::Material::Stone);
  EXPECT_TRUE(door->state == Door::State::Open);

  Feature *empty = manager.getFeature(empty_pos);
  EXPECT_TRUE(empty == nullptr);

  std::cout << "  ✓ getFeature works correctly" << std::endl;
}

// Test FeatureManager property queries
void testFeatureManagerProperties() {
  std::cout << "Testing FeatureManager property queries..." << std::endl;

  FeatureManager manager;
  Position closed_door_pos{1, 1};
  Position open_door_pos{2, 2};
  Position empty_pos{3, 3};

  manager.addFeature(closed_door_pos,
                     Door{Door::Material::Wood, Door::State::Closed});
  manager.addFeature(open_door_pos,
                     Door{Door::Material::Wood, Door::State::Open});

  EXPECT_TRUE(manager.blocksMovement(closed_door_pos));
  EXPECT_FALSE(manager.blocksMovement(open_door_pos));
  EXPECT_FALSE(manager.blocksMovement(empty_pos)); // No feature = doesn't block

  EXPECT_TRUE(manager.blocksLineOfSight(closed_door_pos));
  EXPECT_FALSE(manager.blocksLineOfSight(open_door_pos));
  EXPECT_FALSE(manager.blocksLineOfSight(empty_pos));

  std::cout << "  ✓ Property queries work correctly" << std::endl;
}

// Test FeatureManager replace feature
void testFeatureManagerReplace() {
  std::cout << "Testing FeatureManager feature replacement..." << std::endl;

  FeatureManager manager;
  Position pos{5, 5};

  // Add door
  manager.addFeature(pos, Door{Door::Material::Wood, Door::State::Closed});
  EXPECT_TRUE(isDoor(*manager.getFeature(pos)));

  // Replace with stairs
  manager.addFeature(pos, Stairs{Stairs::Direction::Down, 2});
  EXPECT_TRUE(isStairs(*manager.getFeature(pos)));
  EXPECT_EQ(manager.size(), 1u); // Still only one feature

  std::cout << "  ✓ Feature replacement works correctly" << std::endl;
}

// Test FeatureManager clear
void testFeatureManagerClear() {
  std::cout << "Testing FeatureManager clear..." << std::endl;

  FeatureManager manager;

  manager.addFeature({1, 1}, Door{Door::Material::Wood, Door::State::Closed});
  manager.addFeature({2, 2}, Stairs{Stairs::Direction::Down, 2});
  manager.addFeature({3, 3}, Door{Door::Material::Iron, Door::State::Open});

  EXPECT_EQ(manager.size(), 3u);

  manager.clear();

  EXPECT_EQ(manager.size(), 0u);
  EXPECT_FALSE(manager.hasFeature({1, 1}));
  EXPECT_FALSE(manager.hasFeature({2, 2}));
  EXPECT_FALSE(manager.hasFeature({3, 3}));

  std::cout << "  ✓ Clear works correctly" << std::endl;
}

// Test FeatureManager getAllPositions
void testFeatureManagerGetAllPositions() {
  std::cout << "Testing FeatureManager getAllPositions..." << std::endl;

  FeatureManager manager;

  Position pos1{1, 1};
  Position pos2{5, 10};
  Position pos3{20, 30};

  manager.addFeature(pos1, Door{Door::Material::Wood, Door::State::Closed});
  manager.addFeature(pos2, Stairs{Stairs::Direction::Down, 2});
  manager.addFeature(pos3, Door{Door::Material::Iron, Door::State::Open});

  auto positions = manager.getAllPositions();
  EXPECT_EQ(positions.size(), 3u);

  // Check all positions are present (order doesn't matter with unordered_map)
  bool found_pos1 = false, found_pos2 = false, found_pos3 = false;
  for (const auto &pos : positions) {
    if (pos.x == pos1.x && pos.y == pos1.y)
      found_pos1 = true;
    if (pos.x == pos2.x && pos.y == pos2.y)
      found_pos2 = true;
    if (pos.x == pos3.x && pos.y == pos3.y)
      found_pos3 = true;
  }

  EXPECT_TRUE(found_pos1);
  EXPECT_TRUE(found_pos2);
  EXPECT_TRUE(found_pos3);

  std::cout << "  ✓ getAllPositions works correctly" << std::endl;
}

// Test door state transitions
void testDoorStateTransitions() {
  std::cout << "Testing door state transitions..." << std::endl;

  FeatureManager manager;
  Position door_pos{10, 10};

  // Add closed door
  manager.addFeature(door_pos, Door{Door::Material::Wood, Door::State::Closed});

  EXPECT_TRUE(manager.blocksMovement(door_pos));

  // Open the door (simulate OpenAction)
  Feature *feature = manager.getFeature(door_pos);
  Door *door = getDoor(*feature);
  door->state = Door::State::Open;

  EXPECT_FALSE(manager.blocksMovement(door_pos));
  EXPECT_EQ(getGlyph(*feature), '\'');

  // Close the door again
  door->state = Door::State::Closed;

  EXPECT_TRUE(manager.blocksMovement(door_pos));
  EXPECT_EQ(getGlyph(*feature), '+');

  std::cout << "  ✓ Door state transitions work correctly" << std::endl;
}

int main() {
  std::cout << "\n=== Feature System Tests ===" << std::endl;

  try {
    // Feature type tests
    testDoorStates();
    testStairsDirections();
    testFeatureVariant();

    // Property tests
    testBlocksMovement();
    testBlocksLineOfSight();
    testIsInteractable();
    testGetGlyph();
    testTypeCheckers();
    testGetHelpers();

    // FeatureManager tests
    testFeatureManagerAddRemove();
    testFeatureManagerGet();
    testFeatureManagerProperties();
    testFeatureManagerReplace();
    testFeatureManagerClear();
    testFeatureManagerGetAllPositions();

    // Integration test
    testDoorStateTransitions();

    std::cout << "\n✓ All tests passed!" << std::endl;
    return 0;

  } catch (const std::exception &e) {
    std::cerr << "\n✗ Test failed with exception: " << e.what() << std::endl;
    return 1;
  }
}
