#include "../src/core/Pathfinding.hpp"
#include "../src/core/Position.hpp"
#include "../src/world/Map.hpp"
#include "../src/world/MapViewAdapter.hpp"
#include "../src/world/Tile.hpp"
#include "include/assertions.hpp"
#include <iostream>

int main() {
  using world::Map;
  using world::MapViewAdapter;
  using world::Tile;
  using core::Pathfinding;

  // Create simple test map
  Map m(10, 10, Tile::Floor);

  // Add walls around edges
  for (int x = 0; x < m.width(); ++x) {
    m.set({x, 0}, Tile::Wall);
    m.set({x, m.height() - 1}, Tile::Wall);
  }
  for (int y = 0; y < m.height(); ++y) {
    m.set({0, y}, Tile::Wall);
    m.set({m.width() - 1, y}, Tile::Wall);
  }

  MapViewAdapter view(m);
  Pathfinding pathfinder(view);

  // Test 1: Simple straight path
  auto path1 = pathfinder.findPath({1, 1}, {5, 1});
  EXPECT_TRUE(!path1.empty());
  EXPECT_EQ(path1.front().x, 1);
  EXPECT_EQ(path1.front().y, 1);
  EXPECT_EQ(path1.back().x, 5);
  EXPECT_EQ(path1.back().y, 1);

  // Test 2: Path with obstacle
  m.set({5, 5}, Tile::Wall);
  m.set({5, 6}, Tile::Wall);
  m.set({5, 4}, Tile::Wall);
  auto path2 = pathfinder.findPath({3, 5}, {7, 5});
  EXPECT_TRUE(!path2.empty());

  // Test 3: No path available (completely blocked)
  for (int y = 1; y < 9; ++y)
    m.set({5, y}, Tile::Wall);
  auto path3 = pathfinder.findPath({1, 1}, {8, 8});
  EXPECT_TRUE(path3.empty());

  // Test 4: Same start and goal
  auto path4 = pathfinder.findPath({2, 2}, {2, 2});
  EXPECT_TRUE(!path4.empty());
  EXPECT_EQ(path4.size(), 1);

  std::cout << "Pathfinding tests passed.\n";
  return EXIT_SUCCESS;
}
