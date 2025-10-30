#pragma once
#include "core/Position.hpp"
#include "entities/Entity.hpp"
#include "world/Feature.hpp"
#include "world/Tile.hpp"
#include <memory>
#include <nlohmann/json.hpp>

namespace ai {
class AIBehavior;
}

// Forward declarations
namespace world {
class Map;
class FeatureManager;
} // namespace world

namespace entities {
class EntityManager;
}

// JSON serialization functions for basic game types
// These integrate with nlohmann::json automatic serialization

// Position serialization - must be in core namespace for ADL
namespace core {
void to_json(nlohmann::json &j, const Position &p);
void from_json(const nlohmann::json &j, Position &p);
} // namespace core

// Tile enum serialization - must be in world namespace for ADL
namespace world {
void to_json(nlohmann::json &j, const Tile &t);
void from_json(const nlohmann::json &j, Tile &t);

// Feature variant serialization (Door, Stairs)
void to_json(nlohmann::json &j, const Feature &f);
void from_json(const nlohmann::json &j, Feature &f);

// Map serialization
void to_json(nlohmann::json &j, const Map &m);
void from_json(const nlohmann::json &j, Map &m);

// FeatureManager serialization
void to_json(nlohmann::json &j, const FeatureManager &fm);
void from_json(const nlohmann::json &j, FeatureManager &fm);
} // namespace world

// Entity serialization - must be in entities namespace for ADL
namespace entities {
void to_json(nlohmann::json &j, const Entity &e);
void from_json(const nlohmann::json &j, Entity &e);

// EntityManager serialization
void to_json(nlohmann::json &j, const EntityManager &em);
void from_json(const nlohmann::json &j, EntityManager &em);
} // namespace entities

// AI factory - in global namespace
std::unique_ptr<ai::AIBehavior> createAIFromType(const std::string &ai_type);
