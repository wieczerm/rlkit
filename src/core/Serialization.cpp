#include "Serialization.hpp"
#include "ai/AIBehavior.hpp"
#include "ai/SimpleAI.hpp"
#include "world/FeatureProperties.hpp"
#include "world/TileEnum.hpp"

using json = nlohmann::json;

// Position serialization - in core namespace
namespace core {

void to_json(json &j, const Position &p) { j = json{{"x", p.x}, {"y", p.y}}; }

void from_json(const json &j, Position &p) {
  j.at("x").get_to(p.x);
  j.at("y").get_to(p.y);
}

} // namespace core

// Tile and Feature serialization - in world namespace
namespace world {

void to_json(json &j, const Tile &t) { j = static_cast<int>(t); }

void from_json(const json &j, Tile &t) { t = static_cast<Tile>(j.get<int>()); }

void to_json(json &j, const Feature &f) {
  std::visit(
      [&j](const auto &feature) {
        using T = std::decay_t<decltype(feature)>;

        if constexpr (std::is_same_v<T, Door>) {
          j = json{{"type", "Door"},
                   {"material", static_cast<int>(feature.material)},
                   {"state", static_cast<int>(feature.state)}};
        } else if constexpr (std::is_same_v<T, Stairs>) {
          j = json{{"type", "Stairs"},
                   {"direction", static_cast<int>(feature.direction)},
                   {"target_depth", feature.target_depth}};
        }
      },
      f);
}

void from_json(const json &j, Feature &f) {
  std::string type = j.at("type").get<std::string>();

  if (type == "Door") {
    Door door;
    door.material = static_cast<Door::Material>(j.at("material").get<int>());
    door.state = static_cast<Door::State>(j.at("state").get<int>());
    f = door;
  } else if (type == "Stairs") {
    Stairs stairs;
    stairs.direction =
        static_cast<Stairs::Direction>(j.at("direction").get<int>());
    stairs.target_depth = j.at("target_depth").get<int>();
    f = stairs;
  } else {
    throw std::runtime_error("Unknown feature type: " + type);
  }
}

} // namespace world

// Entity serialization - in entities namespace
namespace entities {

void to_json(json &j, const Entity &e) {
  j["name"] = e.getName();
  j["position"] = e.getPosition(); // ADL will find core::to_json now
  j["glyph"] = std::string(1, e.getGlyph());
  j["properties"] = e.getAllProperties();

  // Serialize AI type if entity has AI
  if (e.hasAI()) {
    // For now, only SimpleAI exists - extend when adding more AI types
    j["ai_type"] = "SimpleAI";
  }
}

void from_json(const json &j, Entity &e) {
  std::string name = j.at("name").get<std::string>();
  core::Position pos = j.at("position").get<core::Position>();
  char glyph = j.at("glyph").get<std::string>()[0];

  // Reconstruct entity using placement new (destructs old, constructs new)
  e.~Entity();
  new (&e) Entity(name, pos);

  e.setGlyph(glyph);

  // Restore properties
  auto properties =
      j.at("properties").get<std::unordered_map<std::string, int>>();
  for (const auto &[key, value] : properties) {
    e.setProperty(key, value);
  }

  // Restore AI if present
  if (j.contains("ai_type")) {
    std::string ai_type = j.at("ai_type").get<std::string>();
    e.setAI(createAIFromType(ai_type));
  }
}

} // namespace entities

// AI factory function - global namespace
std::unique_ptr<ai::AIBehavior> createAIFromType(const std::string &ai_type) {
  if (ai_type == "SimpleAI") {
    return std::make_unique<ai::SimpleAI>();
  }

  // Return nullptr for unknown AI types (entity will have no AI)
  return nullptr;
}
