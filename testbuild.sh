#!/bin/bash

# Build script for serialization tests
# Place this in your project root

g++ -std=c++20 -I. -Isrc \
    src/core/Serialization.cpp \
    src/serialization/GameSerialization.cpp \
    src/world/FeatureManager.cpp \
    src/world/TileRegistry.cpp \
    src/entities/Entity.cpp \
    src/entities/EntityManager.cpp \
    tests/SerializationTests.cpp \
    -o serialization_tests

# Note: Removed these as they're header-only or empty:
# - src/world/TileEnum.cpp (functions are inline in header)
# - src/world/Map.cpp (all methods inline in header) 
# - src/ai/SimpleAI.cpp (not needed for serialization tests)

echo "Build complete. Run with: ./serialization_tests"
