#pragma once
#include "Panel.hpp"
#include <string>

namespace ui {

class StatusBarPanel : public Panel {
public:
  StatusBarPanel(int x, int y, int width);

  std::string render() const override;
  PanelBounds getBounds() const override;

  // Update status info
  void setDepth(int depth) { depth_ = depth; }
  void setTurn(int turn) { turn_ = turn; }
  void setCustomInfo(const std::string &info) { customInfo_ = info; }

private:
  PanelBounds bounds_;
  int depth_;
  int turn_;
  std::string customInfo_;
};

} // namespace ui
