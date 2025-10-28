#pragma once
#include "../core/Position.hpp"
#include <string>
#include <unordered_map>

namespace entities {

class Entity {
public:
  Entity(const std::string &name, const Position &pos);

  // Position management
  const Position &getPosition() const noexcept { return position_; }
  void setPosition(const Position &pos) noexcept { position_ = pos; }

  // Name
  const std::string &getName() const noexcept { return name_; }

  // Generic property system
  void setProperty(const std::string &key, int value);
  int getProperty(const std::string &key, int defaultValue = 0) const;
  bool hasProperty(const std::string &key) const;

  // Common property helpers (optional convenience)
  void setHP(int hp) { setProperty("hp", hp); }
  int getHP() const { return getProperty("hp", 0); }

  void setMaxHP(int maxHp) { setProperty("max_hp", maxHp); }
  int getMaxHP() const { return getProperty("max_hp", 0); }

private:
  std::string name_;
  Position position_;
  std::unordered_map<std::string, int> properties_;
};

} // namespace entities
