
#pragma once
#include <iostream>
#include <cstdlib>

#define EXPECT_TRUE(cond) do { \
  if(!(cond)) { std::cerr << __FILE__ << ":" << __LINE__ \
  << ": EXPECT_TRUE failed: " #cond "\n"; return EXIT_FAILURE; } \
} while(0)

#define EXPECT_EQ(a,b) do { \
  if(!((a)==(b))) { std::cerr << __FILE__ << ":" << __LINE__ \
  << ": EXPECT_EQ failed: " #a " == " #b " (" << (a) << " vs " << (b) << ")\n"; \
  return EXIT_FAILURE; } \
} while(0)

#define TEST_SKIPPED(msg) do { \
  std::cerr << "SKIPPED: " << msg << "\n"; return EXIT_SUCCESS; } while(0)
