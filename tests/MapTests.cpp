#include "../src/core/Position.hpp"
#include "../src/world/Map.hpp"
#include "../src/world/Tile.hpp"
#include "include/assertions.hpp"
#include <iostream>

// Zakładamy interfejs z DESIGN.md:
// Map(int w, int h, Tile fill = Tile::Wall)
// int width() const; int height() const;
// bool in_bounds(Position) const;
// Tile at(Position) const;
// void set(Position, Tile);
// bool isOpaque(int x,int y) const;
// bool blocksMovement(Position) const; bool blocksLineOfSight(Position) const;

int main() {
  using world::Map;
  using world::Tile;
  Position p0{0, 0};

  Map m(10, 8, Tile::Wall);
  EXPECT_EQ(m.width(), 10);
  EXPECT_EQ(m.height(), 8);

  // in_bounds
  EXPECT_TRUE(m.inBounds({0, 0}));
  EXPECT_TRUE(m.inBounds({9, 7}));
  EXPECT_TRUE(!m.inBounds({10, 0}));
  EXPECT_TRUE(!m.inBounds({0, 8}));

  // set/at roundtrip on bounds
  m.set({1, 1}, Tile::Floor);
  EXPECT_EQ(static_cast<int>(m.at({1, 1})), static_cast<int>(Tile::Floor));

  // Opaque / blocks*
  EXPECT_TRUE(m.isOpaque(0, 0)); // ściana domyślna
  EXPECT_TRUE(m.blocksMovement({0, 0}));
  EXPECT_TRUE(m.blocksLineOfSight({0, 0}));

  // Podłoga nie blokuje
  EXPECT_TRUE(!m.blocksMovement({1, 1}));
  EXPECT_TRUE(!m.blocksLineOfSight({1, 1}));

  // Ustalamy kilka płytek
  for (int y = 0; y < m.height(); ++y) {
    for (int x = 0; x < m.width(); ++x) {
      if (x > 0 && y > 0)
        m.set({x, y}, Tile::Floor);
    }
  }
  EXPECT_TRUE(!m.blocksMovement({5, 5}));
  EXPECT_TRUE(!m.blocksLineOfSight({5, 5}));

  std::cout << "Map tests passed.\n";
  return EXIT_SUCCESS;
}
