#pragma once
#include "InputHandler.hpp"
#include "InputScheme.hpp"

namespace core {

class InputMapper {
public:
  InputMapper(Scheme scheme = Scheme::Vi);

  // Map raw char to InputAction
  InputAction mapInput(char key) const;

  // Change scheme at runtime
  void setScheme(Scheme scheme);
  Scheme getCurrentScheme() const { return currentScheme_; }

private:
  Scheme currentScheme_;
  KeyBindings bindings_;
};

} // namespace core
