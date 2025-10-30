#pragma once
#include "Panel.hpp"
#include <string>

namespace ui {

class LookInfoPanel : public Panel {
public:
  LookInfoPanel(int x, int y, int width, int height);

  std::string render() const override;
  PanelBounds getBounds() const override;

  // Update look info
  void setInfo(const std::string &info);
  void clear();

  // Set whether look mode is active
  void setLookMode(bool active) { lookModeActive_ = active; }

private:
  PanelBounds bounds_;
  std::string info_;
  bool lookModeActive_;
};

} // namespace ui
