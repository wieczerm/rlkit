#pragma once
#include "../Map.hpp"
#include "GenOptions.hpp"
#include <random>

namespace world {
struct RoomsOptions : CommonGenOptions {
  int max_rooms = 35;
  int room_min = 5;
  int room_max = 10;
};

// sygnatura modułu – bez zmian
void generateRoomsModule(Map &m, const RoomsOptions &opt, std::mt19937 &rng);
} // namespace world
