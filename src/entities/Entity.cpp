#include "src/entities/Entity.hpp"

namespace entities {

Entity::Entity(const std::string &name, const Position &pos)
    : name_(name), position_(pos) {}

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

} // namespace entities
