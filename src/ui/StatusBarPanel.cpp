#include "StatusBarPanel.hpp"
#include <iomanip>
#include <sstream>

namespace ui {

StatusBarPanel::StatusBarPanel(int x, int y, int width)
    : bounds_{x, y, width, 1}, depth_(1), turn_(0) {}

PanelBounds StatusBarPanel::getBounds() const { return bounds_; }

std::string StatusBarPanel::render() const {
  std::ostringstream oss;

  oss << " Depth: " << depth_ << " | Turn: " << turn_;

  if (!customInfo_.empty()) {
    oss << " | " << customInfo_;
  }

  std::string content = oss.str();

  // Pad to width
  if (content.length() < static_cast<size_t>(bounds_.width)) {
    content += std::string(bounds_.width - content.length(), ' ');
  } else if (content.length() > static_cast<size_t>(bounds_.width)) {
    content = content.substr(0, bounds_.width);
  }

  return content + '\n';
}

} // namespace ui
