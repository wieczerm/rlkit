#include "Entity.hpp"
#include "../ai/AIBehavior.hpp"

namespace entities {

Entity::Entity(const std::string &name, const Position &pos)
    : name_(name), position_(pos) {}

Entity::~Entity() = default;

void Entity::setProperty(const std::string &key, int value) {
  properties_[key] = value;
}

int Entity::getProperty(const std::string &key, int defaultValue) const {
  auto it = properties_.find(key);
  if (it != properties_.end()) {
    return it->second;
  }
  return defaultValue;
}

bool Entity::hasProperty(const std::string &key) const {
  return properties_.find(key) != properties_.end();
}

void Entity::setAI(std::unique_ptr<ai::AIBehavior> ai) { ai_ = std::move(ai); }

} // namespace entities
