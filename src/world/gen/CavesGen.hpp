#pragma once

// Forward declaration
namespace config {
  struct LevelConfig;
  struct CaveGenerationConfig;
}
#include "../Map.hpp"
#include "GenOptions.hpp"
#include <random>

namespace world {
struct CavesOptions : CommonGenOptions {
  int fill_percent = 52; // % ścian na starcie
  int steps = 5;         // iteracje CA
  int birth = 5;
  int survive = 4;
};


// sygnatura modułu – bez zmian
void generateCavesModule(Map &m, const CavesOptions &opt, std::mt19937 &rng);

// New overload using LevelConfig
void generateCavesModule(Map &m, const config::LevelConfig &cfg, std::mt19937 &rng);
} // namespace world
