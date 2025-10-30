#include "MessageLogPanel.hpp"
#include <sstream>

namespace ui {

MessageLogPanel::MessageLogPanel(int x, int y, int width, int height)
    : bounds_{x, y, width, height}, maxMessages_(height) {}

PanelBounds MessageLogPanel::getBounds() const { return bounds_; }

void MessageLogPanel::addMessage(const std::string &message) {
  if (message.empty())
    return;

  // Word wrap if needed
  auto wrapped = wrapText(message, bounds_.width - 2);

  for (const auto &line : wrapped) {
    messages_.push_back(line);

    // Keep only recent messages
    if (messages_.size() > static_cast<size_t>(maxMessages_ * 2)) {
      messages_.pop_front();
    }
  }
}

void MessageLogPanel::clear() { messages_.clear(); }

std::string MessageLogPanel::render() const {
  std::string output;

  // Title
  output += "Messages:\n";
  output += std::string(bounds_.width, '-') + "\n";

  // Show last N messages
  int linesToShow = bounds_.height - 2; // Account for title
  int startIdx = messages_.size() > static_cast<size_t>(linesToShow)
                     ? messages_.size() - linesToShow
                     : 0;

  for (size_t i = startIdx; i < messages_.size(); ++i) {
    output += messages_[i];

    // Pad to width
    if (messages_[i].length() < static_cast<size_t>(bounds_.width)) {
      output += std::string(bounds_.width - messages_[i].length(), ' ');
    }
    output += '\n';
  }

  // Fill remaining lines
  int renderedLines = messages_.size() - startIdx + 2;
  for (int i = renderedLines; i < bounds_.height; ++i) {
    output += std::string(bounds_.width, ' ') + '\n';
  }

  return output;
}

std::vector<std::string> MessageLogPanel::wrapText(const std::string &text,
                                                   int maxWidth) const {
  std::vector<std::string> lines;

  if (text.length() <= static_cast<size_t>(maxWidth)) {
    lines.push_back(text);
    return lines;
  }

  // Simple word wrap
  std::istringstream words(text);
  std::string word;
  std::string currentLine;

  while (words >> word) {
    if (currentLine.empty()) {
      currentLine = word;
    } else if (currentLine.length() + word.length() + 1 <=
               static_cast<size_t>(maxWidth)) {
      currentLine += " " + word;
    } else {
      lines.push_back(currentLine);
      currentLine = word;
    }
  }

  if (!currentLine.empty()) {
    lines.push_back(currentLine);
  }

  return lines;
}

} // namespace ui
