#pragma once
#include "Panel.hpp"
#include <string>

namespace ui {

class StatsBarPanel : public Panel {
public:
  StatsBarPanel(int x, int y, int width);

  std::string render() const override;
  PanelBounds getBounds() const override;

  // Update player stats
  void setPlayerName(const std::string &name) { playerName_ = name; }
  void setHP(int current, int max) {
    currentHP_ = current;
    maxHP_ = max;
  }
  void setHelpHint(const std::string &hint) { helpHint_ = hint; }

private:
  PanelBounds bounds_;
  std::string playerName_;
  int currentHP_;
  int maxHP_;
  std::string helpHint_;

  std::string renderHPBar(int current, int max, int width) const;
};

} // namespace ui
