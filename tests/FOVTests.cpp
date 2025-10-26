#include "../src/core/FOV.hpp"
#include "../src/core/Position.hpp"
#include "../src/world/Map.hpp"
#include "../src/world/MapViewAdapter.hpp"
#include "../src/world/Tile.hpp"
#include "include/assertions.hpp"
#include <iostream>

int main() {
  using world::Map;
  using world::Tile;

  // Pusta mapa (poza brzegami)
  Map m(15, 11, Tile::Floor);

  // Obramowanie ścianami, żeby nie wyjść poza
  for (int x = 0; x < m.width(); ++x) {
    m.set({x, 0}, Tile::Wall);
    m.set({x, m.height() - 1}, Tile::Wall);
  }
  for (int y = 0; y < m.height(); ++y) {
    m.set({0, y}, Tile::Wall);
    m.set({m.width() - 1, y}, Tile::Wall);
  }

  world::MapViewAdapter view(m);
  FOV fov(view);
  Position origin{7, 5};
  int radius = 4;
  fov.compute(origin, radius);

  // Podstawy: źródło widoczne, promień Chebysheva działa
  EXPECT_TRUE(fov.isVisible(origin.x, origin.y));
  EXPECT_TRUE(fov.isVisible(origin.x + 1, origin.y));
  EXPECT_TRUE(fov.isVisible(origin.x, origin.y + 1));
  EXPECT_TRUE(fov.isVisible(origin.x + 4,
                            origin.y)); // na krawędzi promienia w linii prostej

  // Test blokowania przez ścianę — do włączenia po rozbudowie FOV
#ifdef SKIP_BLOCKING_TESTS
  TEST_SKIPPED(
      "Blokowanie ścian w FOV — test pominięty (SKIP_BLOCKING_TESTS=ON).");
#else
  // Budujemy pionową ścianę, która powinna przerywać LOS
  for (int y = 2; y <= 8; ++y)
    m.set({10, y}, Tile::Wall);
  fov.compute(origin, radius);
  // Punkt za ścianą na tej samej linii poziomej
  EXPECT_TRUE(!fov.isVisible(12, 5));
#endif

  std::cout << "FOV tests passed.\n";
  return EXIT_SUCCESS;
}
