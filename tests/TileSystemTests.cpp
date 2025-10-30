#include "../src/world/Tile.hpp"
#include "../src/world/TileProperties.hpp"
#include "../src/world/TileRegistry.hpp"
#include "assertions.hpp"
#include <fstream>
#include <iostream>

using namespace world;

// Test enum to string conversion
void testTileToString() {
  std::cout << "Testing tileToString()..." << std::endl;

  EXPECT_STREQ(tileToString(Tile::OpenGround), "OpenGround");
  EXPECT_STREQ(tileToString(Tile::SolidRock), "SolidRock");
  EXPECT_STREQ(tileToString(Tile::ShallowLiquid), "ShallowLiquid");
  EXPECT_STREQ(tileToString(Tile::DeepLiquid), "DeepLiquid");

  std::cout << "  ✓ All tiles convert to correct strings" << std::endl;
}

// Test string to enum conversion (valid cases)
void testTileFromStringValid() {
  std::cout << "Testing tileFromString() - valid cases..." << std::endl;

  auto og = tileFromString("OpenGround");
  EXPECT_TRUE(og.has_value());
  EXPECT_TRUE(*og == Tile::OpenGround);

  auto sr = tileFromString("SolidRock");
  EXPECT_TRUE(sr.has_value());
  EXPECT_TRUE(*sr == Tile::SolidRock);

  auto sl = tileFromString("ShallowLiquid");
  EXPECT_TRUE(sl.has_value());
  EXPECT_TRUE(*sl == Tile::ShallowLiquid);

  auto dl = tileFromString("DeepLiquid");
  EXPECT_TRUE(dl.has_value());
  EXPECT_TRUE(*dl == Tile::DeepLiquid);

  std::cout << "  ✓ All valid strings convert correctly" << std::endl;
}

// Test string to enum conversion (invalid cases)
void testTileFromStringInvalid() {
  std::cout << "Testing tileFromString() - invalid cases..." << std::endl;

  auto invalid1 = tileFromString("InvalidTile");
  EXPECT_FALSE(invalid1.has_value());

  auto invalid2 = tileFromString("");
  EXPECT_FALSE(invalid2.has_value());

  auto invalid3 = tileFromString("openground"); // case sensitive
  EXPECT_FALSE(invalid3.has_value());

  std::cout << "  ✓ Invalid strings return nullopt" << std::endl;
}

// Test registry hardcoded defaults
void testRegistryDefaults() {
  std::cout << "Testing TileRegistry hardcoded defaults..." << std::endl;

  TileRegistry &reg = TileRegistry::instance();

  // Check all tiles are registered
  EXPECT_TRUE(reg.hasProperties(Tile::OpenGround));
  EXPECT_TRUE(reg.hasProperties(Tile::SolidRock));
  EXPECT_TRUE(reg.hasProperties(Tile::ShallowLiquid));
  EXPECT_TRUE(reg.hasProperties(Tile::DeepLiquid));

  // Verify OpenGround defaults
  const auto &og = reg.getProperties(Tile::OpenGround);
  EXPECT_EQ(og.movement_cost, 100);
  EXPECT_FALSE(og.blocks_los);
  EXPECT_EQ(og.damage_immediate, 0);
  EXPECT_EQ(og.damage_per_turn, 0);

  // Verify SolidRock defaults
  const auto &sr = reg.getProperties(Tile::SolidRock);
  EXPECT_EQ(sr.movement_cost, -1);
  EXPECT_TRUE(sr.blocks_los);
  EXPECT_EQ(sr.damage_immediate, 0);
  EXPECT_EQ(sr.damage_per_turn, 0);

  // Verify ShallowLiquid defaults
  const auto &sl = reg.getProperties(Tile::ShallowLiquid);
  EXPECT_EQ(sl.movement_cost, 200);
  EXPECT_FALSE(sl.blocks_los);

  // Verify DeepLiquid defaults
  const auto &dl = reg.getProperties(Tile::DeepLiquid);
  EXPECT_EQ(dl.movement_cost, -1);
  EXPECT_FALSE(dl.blocks_los);

  std::cout << "  ✓ All default properties correct" << std::endl;
}

// Test wrapper function
void testWrapperFunction() {
  std::cout << "Testing getProperties() wrapper..." << std::endl;

  // Wrapper should call registry correctly
  const auto &props = getProperties(Tile::OpenGround);
  EXPECT_EQ(props.movement_cost, 100);

  const auto &props2 = getProperties(Tile::SolidRock);
  EXPECT_EQ(props2.movement_cost, -1);

  std::cout << "  ✓ Wrapper function works correctly" << std::endl;
}

// Test JSON loading with test config
void testJSONLoading() {
  std::cout << "Testing JSON loading..." << std::endl;

  // Create fresh registry instance for testing
  // Note: Since we use singleton, we're modifying the global instance
  // In production, tests should be isolated, but for now this demonstrates
  // functionality

  TileRegistry &reg = TileRegistry::instance();

  // Load test config with distinctive values
  reg.loadFromJSON("../test_tiles.json");

  // Verify test values were loaded (overriding defaults)
  const auto &og = reg.getProperties(Tile::OpenGround);
  EXPECT_EQ(og.movement_cost, 150);  // test value, not default 100
  EXPECT_EQ(og.damage_immediate, 5); // test value, not default 0
  EXPECT_EQ(og.damage_per_turn, 1);  // test value, not default 0

  const auto &sl = reg.getProperties(Tile::ShallowLiquid);
  EXPECT_EQ(sl.movement_cost, 300); // test value, not default 200
  EXPECT_TRUE(sl.blocks_los);       // test value, not default false
  EXPECT_EQ(sl.damage_immediate, 2);
  EXPECT_EQ(sl.damage_per_turn, 3);

  std::cout << "  ✓ JSON values loaded and override defaults" << std::endl;
}

// Test graceful failure on missing file
void testMissingFile() {
  std::cout << "Testing missing file handling..." << std::endl;

  TileRegistry &reg = TileRegistry::instance();

  // Should not throw, should print warning to stderr
  reg.loadFromJSON("nonexistent_file.json");

  // Should still have defaults available
  EXPECT_TRUE(reg.hasProperties(Tile::OpenGround));

  std::cout << "  ✓ Missing file handled gracefully" << std::endl;
}

// Test malformed JSON handling
void testMalformedJSON() {
  std::cout << "Testing malformed JSON handling..." << std::endl;

  // Create temporary malformed JSON
  {
    std::ofstream temp("tests/malformed.json");
    temp << "{ this is not valid json }";
  }

  TileRegistry &reg = TileRegistry::instance();

  // Should not throw, should print warning
  reg.loadFromJSON("tests/malformed.json");

  // Should still have defaults
  EXPECT_TRUE(reg.hasProperties(Tile::SolidRock));

  std::cout << "  ✓ Malformed JSON handled gracefully" << std::endl;
}

// Test unknown tile in JSON
void testUnknownTileInJSON() {
  std::cout << "Testing unknown tile type in JSON..." << std::endl;

  // Create JSON with unknown tile
  {
    std::ofstream temp("tests/unknown_tile.json");
    temp << R"({
            "tiles": {
                "UnknownTileType": {
                    "movement_cost": 999,
                    "blocks_los": false,
                    "damage_immediate": 0,
                    "damage_per_turn": 0
                }
            }
        })";
  }

  TileRegistry &reg = TileRegistry::instance();

  // Should print warning but not crash
  reg.loadFromJSON("tests/unknown_tile.json");

  std::cout << "  ✓ Unknown tile type skipped with warning" << std::endl;
}

int main() {
  std::cout << "\n=== Tile System Tests ===" << std::endl;

  try {
    // Enum conversion tests
    testTileToString();
    testTileFromStringValid();
    testTileFromStringInvalid();

    // Registry tests
    testRegistryDefaults();
    testWrapperFunction();

    // JSON loading tests
    testJSONLoading();
    testMissingFile();
    testMalformedJSON();
    testUnknownTileInJSON();

    std::cout << "\n✓ All tests passed!" << std::endl;
    return 0;

  } catch (const std::exception &e) {
    std::cerr << "\n✗ Test failed with exception: " << e.what() << std::endl;
    return 1;
  }
}
