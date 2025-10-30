#include "InputMapper.hpp"

namespace core {

InputMapper::InputMapper(Scheme scheme) : currentScheme_(scheme) {
  bindings_ = InputScheme::getScheme(scheme);
}

InputAction InputMapper::mapInput(char key) const {
  auto it = bindings_.find(key);
  if (it != bindings_.end()) {
    return it->second;
  }
  return InputAction::None;
}

void InputMapper::setScheme(Scheme scheme) {
  currentScheme_ = scheme;
  bindings_ = InputScheme::getScheme(scheme);
}

} // namespace core
