#include "../src/ai/SimpleAI.hpp"
#include "../src/core/Serialization.hpp"
#include "../src/serialization/GameSerialization.hpp"
#include "../src/world/FeatureProperties.hpp" // ADD THIS - for getDoor, getStairs, etc
#include "assertions.hpp"
#include <fstream>
#include <iostream>

using namespace core;
using namespace world;
using namespace entities;
using namespace serialization;
using json = nlohmann::json;

// Test Position serialization
void testPositionSerialization() {
  std::cout << "Testing Position serialization..." << std::endl;

  Position original{42, 17};
  json j = original;
  Position restored = j.get<Position>();

  EXPECT_EQ(restored.x, 42);
  EXPECT_EQ(restored.y, 17);

  std::cout << "  ✓ Position round-trip successful" << std::endl;
}

// Test Tile serialization
void testTileSerialization() {
  std::cout << "Testing Tile serialization..." << std::endl;

  Tile original = Tile::SolidRock;
  json j = original;
  Tile restored = j.get<Tile>();

  EXPECT_TRUE(restored == Tile::SolidRock);

  std::cout << "  ✓ Tile round-trip successful" << std::endl;
}

// Test Door serialization
void testDoorSerialization() {
  std::cout << "Testing Door serialization..." << std::endl;

  Feature original = Door{Door::Material::Iron, Door::State::Closed};
  json j = original;
  Feature restored = j.get<Feature>();

  EXPECT_TRUE(std::holds_alternative<Door>(restored));

  const Door *door = getDoor(restored);
  EXPECT_TRUE(door != nullptr);
  EXPECT_TRUE(door->material == Door::Material::Iron);
  EXPECT_TRUE(door->state == Door::State::Closed);

  std::cout << "  ✓ Door round-trip successful" << std::endl;
}

// Test Stairs serialization
void testStairsSerialization() {
  std::cout << "Testing Stairs serialization..." << std::endl;

  Feature original = Stairs{Stairs::Direction::Down, 5};
  json j = original;
  Feature restored = j.get<Feature>();

  EXPECT_TRUE(std::holds_alternative<Stairs>(restored));

  const Stairs *stairs = getStairs(restored);
  EXPECT_TRUE(stairs != nullptr);
  EXPECT_TRUE(stairs->direction == Stairs::Direction::Down);
  EXPECT_EQ(stairs->target_depth, 5);

  std::cout << "  ✓ Stairs round-trip successful" << std::endl;
}

// Test Entity serialization (without AI)
void testEntitySerializationNoAI() {
  std::cout << "Testing Entity serialization (no AI)..." << std::endl;

  Entity original("TestMonster", Position{10, 20});
  original.setGlyph('T');
  original.setProperty("hp", 50);
  original.setProperty("str", 12);

  json j = original;
  Entity restored("", Position{0, 0});
  j.get_to(restored);

  EXPECT_TRUE(restored.getName() == "TestMonster");
  EXPECT_EQ(restored.getPosition().x, 10);
  EXPECT_EQ(restored.getPosition().y, 20);
  EXPECT_EQ(restored.getGlyph(), 'T');
  EXPECT_EQ(restored.getProperty("hp"), 50);
  EXPECT_EQ(restored.getProperty("str"), 12);
  EXPECT_FALSE(restored.hasAI());

  std::cout << "  ✓ Entity (no AI) round-trip successful" << std::endl;
}

// Test Entity serialization (with AI)
void testEntitySerializationWithAI() {
  std::cout << "Testing Entity serialization (with AI)..." << std::endl;

  Entity original("Goblin", Position{5, 8});
  original.setGlyph('g');
  original.setProperty("hp", 20);
  original.setAI(std::make_unique<ai::SimpleAI>());

  json j = original;
  Entity restored("", Position{0, 0});
  j.get_to(restored);

  EXPECT_TRUE(restored.getName() == "Goblin");
  EXPECT_EQ(restored.getPosition().x, 5);
  EXPECT_EQ(restored.getPosition().y, 8);
  EXPECT_EQ(restored.getGlyph(), 'g');
  EXPECT_EQ(restored.getProperty("hp"), 20);
  EXPECT_TRUE(restored.hasAI());

  std::cout << "  ✓ Entity (with AI) round-trip successful" << std::endl;
}

// Test Map serialization
void testMapSerialization() {
  std::cout << "Testing Map serialization..." << std::endl;

  Map original(10, 8, Tile::OpenGround);
  original.set({0, 0}, Tile::SolidRock);
  original.set({5, 4}, Tile::SolidRock);
  original.set({9, 7}, Tile::ShallowLiquid);

  json j;
  world::to_json(j, original);   // Explicit call
  Map restored(1, 1);            // Will be resized during deserialization
  world::from_json(j, restored); // Explicit call

  EXPECT_EQ(restored.width(), 10);
  EXPECT_EQ(restored.height(), 8);
  EXPECT_TRUE(restored.at({0, 0}) == Tile::SolidRock);
  EXPECT_TRUE(restored.at({5, 4}) == Tile::SolidRock);
  EXPECT_TRUE(restored.at({9, 7}) == Tile::ShallowLiquid);
  EXPECT_TRUE(restored.at({1, 1}) == Tile::OpenGround);

  std::cout << "  ✓ Map round-trip successful" << std::endl;
}

// Test FeatureManager serialization
void testFeatureManagerSerialization() {
  std::cout << "Testing FeatureManager serialization..." << std::endl;

  FeatureManager original;
  original.addFeature({5, 5}, Door{Door::Material::Wood, Door::State::Open});
  original.addFeature({10, 10}, Stairs{Stairs::Direction::Down, 2});
  original.addFeature({15, 3}, Door{Door::Material::Iron, Door::State::Closed});

  json j;
  world::to_json(j, original); // Explicit call
  FeatureManager restored;
  world::from_json(j, restored); // Explicit call

  EXPECT_EQ(restored.size(), 3u);
  EXPECT_TRUE(restored.hasFeature({5, 5}));
  EXPECT_TRUE(restored.hasFeature({10, 10}));
  EXPECT_TRUE(restored.hasFeature({15, 3}));

  // Verify door at {5,5}
  const Feature *f1 = restored.getFeature({5, 5});
  EXPECT_TRUE(isDoor(*f1));
  const Door *door1 = getDoor(*f1);
  EXPECT_TRUE(door1->state == Door::State::Open);

  // Verify stairs at {10,10}
  const Feature *f2 = restored.getFeature({10, 10});
  EXPECT_TRUE(isStairs(*f2));

  std::cout << "  ✓ FeatureManager round-trip successful" << std::endl;
}

// Test EntityManager serialization
void testEntityManagerSerialization() {
  std::cout << "Testing EntityManager serialization..." << std::endl;

  EntityManager original;

  auto e1 = std::make_unique<Entity>("Player", Position{5, 5});
  e1->setGlyph('@');
  e1->setProperty("hp", 100);
  original.addEntity(std::move(e1));

  auto e2 = std::make_unique<Entity>("Goblin", Position{10, 10});
  e2->setGlyph('g');
  e2->setProperty("hp", 20);
  e2->setAI(std::make_unique<ai::SimpleAI>());
  original.addEntity(std::move(e2));

  json j;
  entities::to_json(j, original); // Explicit call
  EntityManager restored;
  entities::from_json(j, restored); // Explicit call

  const auto &entities = restored.getEntities(); // Changed from getAllEntities
  EXPECT_EQ(entities.size(), 2u);

  // Check entities exist (order not guaranteed)
  Entity *player = restored.getEntityAt({5, 5});
  Entity *goblin = restored.getEntityAt({10, 10});

  EXPECT_TRUE(player != nullptr);
  EXPECT_TRUE(goblin != nullptr);
  EXPECT_TRUE(player->getName() == "Player");
  EXPECT_TRUE(goblin->getName() == "Goblin");
  EXPECT_EQ(player->getProperty("hp"), 100);
  EXPECT_EQ(goblin->getProperty("hp"), 20);
  EXPECT_TRUE(goblin->hasAI());

  std::cout << "  ✓ EntityManager round-trip successful" << std::endl;
}

// Test LevelState serialization
void testLevelStateSerialization() {
  std::cout << "Testing LevelState serialization..." << std::endl;

  Map map(20, 15, Tile::OpenGround);
  map.set({5, 5}, Tile::SolidRock);

  FeatureManager features;
  features.addFeature({10, 10},
                      Door{Door::Material::Wood, Door::State::Closed});

  EntityManager entities_mgr;
  auto entity = std::make_unique<Entity>("Test", Position{3, 3});
  entities_mgr.addEntity(std::move(entity));

  std::vector<bool> discovered(20 * 15, false);
  discovered[0] = true; // Mark first tile as discovered
  discovered[10] = true;

  LevelState original(std::move(map), std::move(features),
                      std::move(entities_mgr), std::move(discovered), 5);

  json j;
  serialization::to_json(j, original); // Explicit call

  // Deserialize to new components then construct LevelState
  Map restored_map(1, 1);
  FeatureManager restored_features;
  EntityManager restored_entities;
  std::vector<bool> restored_discovered;
  int restored_depth = 0;

  j.at("depth").get_to(restored_depth);
  world::from_json(j.at("map"), restored_map);
  world::from_json(j.at("features"), restored_features);
  entities::from_json(j.at("entities"), restored_entities);
  j.at("discovered").get_to(restored_discovered);

  LevelState restored(std::move(restored_map), std::move(restored_features),
                      std::move(restored_entities),
                      std::move(restored_discovered), restored_depth);

  EXPECT_EQ(restored.depth, 5);
  EXPECT_EQ(restored.map.width(), 20);
  EXPECT_EQ(restored.map.height(), 15);
  EXPECT_TRUE(restored.map.at({5, 5}) == Tile::SolidRock);
  EXPECT_EQ(restored.features.size(), 1u);
  EXPECT_EQ(restored.entities.getEntities().size(),
            1u); // Changed from getAllEntities
  EXPECT_EQ(restored.discovered.size(), 300u);
  EXPECT_TRUE(restored.discovered[0]);
  EXPECT_TRUE(restored.discovered[10]);
  EXPECT_FALSE(restored.discovered[5]);

  std::cout << "  ✓ LevelState round-trip successful" << std::endl;
}

// Test SaveData serialization
void testSaveDataSerialization() {
  std::cout << "Testing SaveData serialization..." << std::endl;

  SaveData original;
  original.turn_counter = 1000;
  original.current_level_index = 1;

  // Add two levels
  Map map1(10, 10, Tile::OpenGround);
  FeatureManager features1;
  EntityManager entities1;
  std::vector<bool> discovered1(100, false);
  original.visited_levels.push_back(
      LevelState(std::move(map1), std::move(features1), std::move(entities1),
                 std::move(discovered1), 1));

  Map map2(15, 12, Tile::SolidRock);
  FeatureManager features2;
  EntityManager entities2;
  std::vector<bool> discovered2(180, true);
  original.visited_levels.push_back(
      LevelState(std::move(map2), std::move(features2), std::move(entities2),
                 std::move(discovered2), 2));

  json j;
  serialization::to_json(j, original); // Explicit call
  SaveData restored;
  serialization::from_json(j, restored); // Explicit call

  EXPECT_EQ(restored.turn_counter, 1000);
  EXPECT_EQ(restored.current_level_index, 1);
  EXPECT_EQ(restored.visited_levels.size(), 2u);
  EXPECT_EQ(restored.visited_levels[0].depth, 1);
  EXPECT_EQ(restored.visited_levels[1].depth, 2);
  EXPECT_EQ(restored.visited_levels[0].map.width(), 10);
  EXPECT_EQ(restored.visited_levels[1].map.width(), 15);

  std::cout << "  ✓ SaveData round-trip successful" << std::endl;
}

// Test save/load to file
void testSaveLoadFile() {
  std::cout << "Testing save/load to file..." << std::endl;

  const std::string filename = "test_save.json";

  // Create save data
  SaveData original;
  original.turn_counter = 500;
  original.current_level_index = 0;

  Map map(8, 6, Tile::OpenGround);
  map.set({3, 3}, Tile::SolidRock);

  FeatureManager features;
  features.addFeature({4, 4}, Stairs{Stairs::Direction::Down, 2});

  EntityManager entities;
  auto entity = std::make_unique<Entity>("Hero", Position{1, 1});
  entity->setProperty("hp", 75);
  entities.addEntity(std::move(entity));

  std::vector<bool> discovered(48, false);
  discovered[0] = true;

  original.visited_levels.push_back(
      LevelState(std::move(map), std::move(features), std::move(entities),
                 std::move(discovered), 1));

  // Save to file
  saveGame(original, filename);

  // Verify file exists
  {
    std::ifstream check(filename);
    EXPECT_TRUE(check.good());
  }

  // Load from file
  SaveData restored = loadGame(filename);

  EXPECT_EQ(restored.turn_counter, 500);
  EXPECT_EQ(restored.current_level_index, 0);
  EXPECT_EQ(restored.visited_levels.size(), 1u);
  EXPECT_EQ(restored.visited_levels[0].depth, 1);
  EXPECT_EQ(restored.visited_levels[0].map.width(), 8);
  EXPECT_TRUE(restored.visited_levels[0].map.at({3, 3}) == Tile::SolidRock);
  EXPECT_EQ(restored.visited_levels[0].features.size(), 1u);
  EXPECT_EQ(restored.visited_levels[0].entities.getEntities().size(),
            1u); // Changed from getAllEntities

  // Clean up test file
  std::remove(filename.c_str());

  std::cout << "  ✓ Save/load file successful" << std::endl;
}

// Test error handling for missing file
void testLoadMissingFile() {
  std::cout << "Testing load missing file error..." << std::endl;

  bool caught_exception = false;
  try {
    SaveData data = loadGame("nonexistent_save.json");
  } catch (const std::runtime_error &e) {
    caught_exception = true;
    std::string msg = e.what();
    EXPECT_TRUE(msg.find("Failed to open") != std::string::npos);
  }

  EXPECT_TRUE(caught_exception);

  std::cout << "  ✓ Missing file error handled correctly" << std::endl;
}

int main() {
  std::cout << "\n=== Serialization System Tests ===" << std::endl;

  try {
    // Basic type serialization
    testPositionSerialization();
    testTileSerialization();
    testDoorSerialization();
    testStairsSerialization();

    // Entity serialization
    testEntitySerializationNoAI();
    testEntitySerializationWithAI();

    // Manager serialization
    testMapSerialization();
    testFeatureManagerSerialization();
    testEntityManagerSerialization();

    // Game state serialization
    testLevelStateSerialization();
    testSaveDataSerialization();

    // File I/O
    testSaveLoadFile();
    testLoadMissingFile();

    std::cout << "\n✓ All tests passed!" << std::endl;
    return 0;

  } catch (const std::exception &e) {
    std::cerr << "\n✗ Test failed with exception: " << e.what() << std::endl;
    return 1;
  }
}
