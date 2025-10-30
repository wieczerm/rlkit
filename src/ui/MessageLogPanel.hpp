#pragma once
#include "Panel.hpp"
#include <deque>
#include <string>
#include <vector>

namespace ui {

class MessageLogPanel : public Panel {
public:
  MessageLogPanel(int x, int y, int width, int height);

  std::string render() const override;
  PanelBounds getBounds() const override;

  // Add new message (auto-scrolls if needed)
  void addMessage(const std::string &message);

  // Clear all messages
  void clear();

  // Get recent messages
  const std::deque<std::string> &getMessages() const { return messages_; }

private:
  PanelBounds bounds_;
  std::deque<std::string> messages_;
  int maxMessages_; // Based on height

  // Word wrap helper
  std::vector<std::string> wrapText(const std::string &text,
                                    int maxWidth) const;
};

} // namespace ui
