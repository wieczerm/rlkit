#include "Panel.hpp"
#include "UIManager.hpp"
#include <iostream>

namespace ui {

UIManager::UIManager(int termWidth, int termHeight)
    : termWidth_(termWidth), termHeight_(termHeight) {}

void UIManager::addPanel(std::unique_ptr<Panel> panel) {
  panels_.push_back(std::move(panel));
}

void UIManager::render() const {
  clear();

  for (const auto &panel : panels_) {
    auto bounds = panel->getBounds();
    std::string content = panel->render();

    // Position cursor at panel origin and print
    std::cout << "\033[" << (bounds.y + 1) << ";" << (bounds.x + 1) << "H";

    // Print content line by line
    size_t pos = 0;
    int currentLine = 0;

    while (pos < content.length() && currentLine < bounds.height) {
      size_t lineEnd = content.find('\n', pos);
      if (lineEnd == std::string::npos) {
        lineEnd = content.length();
      }

      std::string line = content.substr(pos, lineEnd - pos);

      // Position cursor for this line
      std::cout << "\033[" << (bounds.y + currentLine + 1) << ";"
                << (bounds.x + 1) << "H";
      std::cout << line;

      pos = lineEnd + 1;
      currentLine++;
    }
  }

  // Move cursor to bottom
  std::cout << "\033[" << termHeight_ << ";1H";
  std::cout.flush();
}

void UIManager::clear() const {
  std::cout << "\033[2J\033[H"; // Clear screen, move to home
}

template <typename T> T *UIManager::getPanel() {
  for (auto &panel : panels_) {
    if (auto *typed = dynamic_cast<T *>(panel.get())) {
      return typed;
    }
  }
  return nullptr;
}

} // namespace ui
