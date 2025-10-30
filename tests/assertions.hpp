#pragma once
#include <cstring>
#include <iostream>
#include <stdexcept>

#define EXPECT_TRUE(condition)                                                 \
  do {                                                                         \
    if (!(condition)) {                                                        \
      throw std::runtime_error("EXPECT_TRUE failed: " #condition);             \
    }                                                                          \
  } while (0)

#define EXPECT_FALSE(condition)                                                \
  do {                                                                         \
    if (condition) {                                                           \
      throw std::runtime_error("EXPECT_FALSE failed: " #condition);            \
    }                                                                          \
  } while (0)

#define EXPECT_EQ(a, b)                                                        \
  do {                                                                         \
    if ((a) != (b)) {                                                          \
      throw std::runtime_error("EXPECT_EQ failed: " #a " != " #b);             \
    }                                                                          \
  } while (0)

#define EXPECT_STREQ(a, b)                                                     \
  do {                                                                         \
    if (std::strcmp((a), (b)) != 0) {                                          \
      throw std::runtime_error("EXPECT_STREQ failed: " #a " != " #b);          \
    }                                                                          \
  } while (0)
