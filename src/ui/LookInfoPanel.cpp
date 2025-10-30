#include "LookInfoPanel.hpp"
#include <sstream>

namespace ui {

LookInfoPanel::LookInfoPanel(int x, int y, int width, int height)
    : bounds_{x, y, width, height}, lookModeActive_(false) {}

PanelBounds LookInfoPanel::getBounds() const { return bounds_; }

void LookInfoPanel::setInfo(const std::string &info) { info_ = info; }

void LookInfoPanel::clear() { info_.clear(); }

std::string LookInfoPanel::render() const {
  std::string output;

  // Title
  if (lookModeActive_) {
    output += "Look At: [ACTIVE]\n";
  } else {
    output += "Look At: (press x)\n";
  }
  output += std::string(bounds_.width, '-') + "\n";

  // Info content
  if (!info_.empty()) {
    // Simple line break at width
    size_t pos = 0;
    int linesUsed = 2; // Title + separator

    while (pos < info_.length() && linesUsed < bounds_.height) {
      size_t lineEnd = std::min(pos + bounds_.width, info_.length());
      output += info_.substr(pos, lineEnd - pos);

      // Pad line
      if (lineEnd - pos < static_cast<size_t>(bounds_.width)) {
        output += std::string(bounds_.width - (lineEnd - pos), ' ');
      }
      output += '\n';

      pos = lineEnd;
      linesUsed++;
    }

    // Fill remaining
    for (int i = linesUsed; i < bounds_.height; ++i) {
      output += std::string(bounds_.width, ' ') + '\n';
    }
  } else {
    // Empty state
    for (int i = 2; i < bounds_.height; ++i) {
      output += std::string(bounds_.width, ' ') + '\n';
    }
  }

  return output;
}

} // namespace ui
