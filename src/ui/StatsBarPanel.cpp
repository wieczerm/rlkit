#include "StatsBarPanel.hpp"
#include <iomanip>
#include <sstream>

namespace ui {

StatsBarPanel::StatsBarPanel(int x, int y, int width)
    : bounds_{x, y, width, 1}, playerName_("Player"), currentHP_(100),
      maxHP_(100), helpHint_("? for help") {}

PanelBounds StatsBarPanel::getBounds() const { return bounds_; }

std::string StatsBarPanel::render() const {
  std::ostringstream oss;

  // Player name and HP
  oss << " " << playerName_ << " HP:";

  // HP bar (10 chars)
  std::string hpBar = renderHPBar(currentHP_, maxHP_, 10);
  oss << hpBar;

  oss << " " << currentHP_ << "/" << maxHP_;

  std::string leftSide = oss.str();

  // Right side - help hint
  int padding = bounds_.width - leftSide.length() - helpHint_.length() - 1;
  if (padding < 0)
    padding = 0;

  return leftSide + std::string(padding, ' ') + helpHint_ + '\n';
}

std::string StatsBarPanel::renderHPBar(int current, int max, int width) const {
  if (max <= 0)
    return std::string(width, '?');

  int filled = (current * width) / max;
  if (filled < 0)
    filled = 0;
  if (filled > width)
    filled = width;

  std::string bar = "[";
  bar += std::string(filled, '=');
  bar += std::string(width - filled, ' ');
  bar += "]";

  return bar;
}

} // namespace ui
