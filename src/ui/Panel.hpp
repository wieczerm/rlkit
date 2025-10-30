#pragma once
#include <string>

namespace ui {

struct PanelBounds {
  int x, y; // Top-left position
  int width, height;
};

class Panel {
public:
  virtual ~Panel() = default;

  // Render panel content to string
  virtual std::string render() const = 0;

  // Get panel dimensions
  virtual PanelBounds getBounds() const = 0;

  // Optional: update panel state
  virtual void update() {}
};

} // namespace ui
