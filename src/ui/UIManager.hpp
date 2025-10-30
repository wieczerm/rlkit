#pragma once
#include <memory>
#include <vector>

namespace ui {

class Panel;

class UIManager {
public:
  UIManager(int termWidth = 80, int termHeight = 24);

  // Register panels
  void addPanel(std::unique_ptr<Panel> panel);

  // Render all panels to terminal
  void render() const;

  // Clear screen
  void clear() const;

  // Get panel by type (for updates)
  template <typename T> T *getPanel();

private:
  int termWidth_;
  int termHeight_;
  std::vector<std::unique_ptr<Panel>> panels_;
};

} // namespace ui
