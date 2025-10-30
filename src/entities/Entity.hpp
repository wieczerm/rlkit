#pragma once
#include "core/Position.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace ai {
class AIBehavior;
}

namespace entities {

class Entity {
public:
  Entity(const std::string &name, const core::Position &pos);
  ~Entity();
  // Position management
  const core::Position &getPosition() const noexcept { return position_; }
  void setPosition(const core::Position &pos) noexcept { position_ = pos; }

  // Name
  const std::string &getName() const noexcept { return name_; }

  char getGlyph() const { return glyph_; }
  void setGlyph(char g) { glyph_ = g; }
  const std::unordered_map<std::string, int> &getAllProperties() const;

  // Generic property system
  void setProperty(const std::string &key, int value);
  int getProperty(const std::string &key, int defaultValue = 0) const;
  bool hasProperty(const std::string &key) const;

  // Common property helpers (optional convenience)
  void setHP(int hp) { setProperty("hp", hp); }
  int getHP() const { return getProperty("hp", 0); }

  void setMaxHP(int maxHp) { setProperty("max_hp", maxHp); }
  int getMaxHP() const { return getProperty("max_hp", 0); }

  // AI
  void setAI(std::unique_ptr<ai::AIBehavior> ai);
  bool hasAI() const { return ai_ != nullptr; }
  ai::AIBehavior *getAI() { return ai_.get(); }
  const ai::AIBehavior *getAI() const { return ai_.get(); }

private:
  std::string name_;
  core::Position position_;
  std::unordered_map<std::string, int> properties_;
  std::unique_ptr<ai::AIBehavior> ai_;
  char glyph_;
};

} // namespace entities
