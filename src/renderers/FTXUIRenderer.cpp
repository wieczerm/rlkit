#include "FTXUIRenderer.hpp"
#include "core/FOV.hpp"
#include "entities/Entity.hpp"
#include "entities/EntityManager.hpp"
#include "world/FeatureManager.hpp"
#include "world/FeatureProperties.hpp"
#include "world/Map.hpp"
#include "world/Tile.hpp"

#include <algorithm>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>
#include <sstream>

using namespace ftxui;

namespace renderers {

FTXUIRenderer::FTXUIRenderer(int gameViewWidth, int gameViewHeight)
    : gameViewWidth_(gameViewWidth), gameViewHeight_(gameViewHeight) {}

void FTXUIRenderer::render(const GameState &state) {
  // Build layout
  auto document = vbox(
      {buildStatusBar(state), separator(),
       hbox({buildGameView(state) | flex, separator(),
             vbox({buildMessageLog(state) | size(HEIGHT, EQUAL, 8), separator(),
                   buildLookInfo(state) | size(HEIGHT, EQUAL, 6), separator(),
                   buildMinimap(state) | flex}) |
                 size(WIDTH, EQUAL, 30)}) |
           flex,
       separator(), buildStatsBar(state)});

  // Render to screen
  // auto screen = Screen::Create(Dimension::Full());
  auto screen = Screen::Create(Dimension::Fixed(80), Dimension::Fixed(24));
  std::cout << screen.ResetPosition();
  Render(screen, document);
  screen.Print();
  std::cout << std::flush;
}

Element FTXUIRenderer::buildStatusBar(const GameState &state) {
  std::ostringstream oss;
  oss << " Depth: " << state.depth << " | Turn: " << state.turn;

  return hbox({text(oss.str()) | bold, filler()}) | bgcolor(Color::Blue) |
         color(Color::White);
}

Element FTXUIRenderer::buildGameView(const GameState &state) {
  if (!state.map || !state.fov || !state.entities) {
    return window(text("Game View"), text("Loading..."));
  }

  std::vector<std::string> lines;

  int startX = state.cameraCenter.x - gameViewWidth_ / 2;
  int startY = state.cameraCenter.y - gameViewHeight_ / 2;

  for (int y = 0; y < gameViewHeight_; ++y) {
    std::string line;
    for (int x = 0; x < gameViewWidth_; ++x) {
      int worldX = startX + x;
      int worldY = startY + y;

      if (!state.map->inBounds({worldX, worldY}) ||
          !state.fov->isVisible(worldX, worldY)) {
        line += ' ';
        continue;
      }

      // Cursor
      if (state.cursor && state.cursor->x == worldX &&
          state.cursor->y == worldY) {
        line += 'X';
        continue;
      }

      // Entity
      entities::Entity *ent = state.entities->getEntityAt({worldX, worldY});
      if (ent) {
        line += ent->getGlyph();
        continue;
      }

      // Feature (doors, stairs)
      if (state.features) {
        const world::Feature *feature =
            state.features->getFeature({worldX, worldY});
        if (feature) {
          line += world::getGlyph(*feature);
          continue;
        }
      }

      // Tile (floor, wall)
      line += tileToChar(state.map->at({worldX, worldY}));
    }
    lines.push_back(line);
  }

  // Convert to FTXUI elements
  std::vector<Element> lineElements;
  for (const auto &line : lines) {
    lineElements.push_back(text(line));
  }

  return window(text(""), vbox(std::move(lineElements)));
}

Element FTXUIRenderer::buildMessageLog(const GameState &state) {
  std::vector<Element> messageElements;

  const int maxLines = 6;  // Available lines in panel
  const int maxWidth = 28; // Panel width - 2 for padding

  std::vector<std::string> wrappedMessages;

  // Process messages from newest to oldest, applying word wrap
  for (int i = state.messages.size() - 1; i >= 0; --i) {
    const auto &msg = state.messages[i];

    // Simple word wrap
    if (msg.length() <= static_cast<size_t>(maxWidth)) {
      wrappedMessages.insert(wrappedMessages.begin(), msg);
    } else {
      // Split long message
      size_t pos = 0;
      std::vector<std::string> lines;
      while (pos < msg.length()) {
        size_t end = std::min(pos + maxWidth, msg.length());

        // Try to break at word boundary
        if (end < msg.length() && msg[end] != ' ') {
          size_t lastSpace = msg.rfind(' ', end);
          if (lastSpace != std::string::npos && lastSpace > pos) {
            end = lastSpace;
          }
        }

        lines.push_back(msg.substr(pos, end - pos));
        pos = end;
        if (pos < msg.length() && msg[pos] == ' ')
          pos++; // Skip space
      }

      // Insert in reverse (oldest first)
      wrappedMessages.insert(wrappedMessages.begin(), lines.begin(),
                             lines.end());
    }

    // Stop if we have enough lines
    if (static_cast<int>(wrappedMessages.size()) >= maxLines * 2) {
      break;
    }
  }

  // Take only last N lines that fit
  int startIdx = wrappedMessages.size() > static_cast<size_t>(maxLines)
                     ? wrappedMessages.size() - maxLines
                     : 0;

  for (size_t i = startIdx; i < wrappedMessages.size(); ++i) {
    messageElements.push_back(text(wrappedMessages[i]));
  }

  // Fill empty space if needed
  while (static_cast<int>(messageElements.size()) < maxLines) {
    messageElements.push_back(text(""));
  }

  return window(text("Messages"), vbox(std::move(messageElements)));
}
Element FTXUIRenderer::buildLookInfo(const GameState &state) {
  std::vector<Element> infoLines;

  if (state.lookModeActive) {
    infoLines.push_back(text("Look Mode: ACTIVE") | bold |
                        color(Color::Yellow));
    infoLines.push_back(separator());

    if (!state.lookInfo.empty()) {
      // Split info into multiple lines if needed
      std::istringstream iss(state.lookInfo);
      std::string word;
      std::string currentLine;

      while (iss >> word) {
        if (currentLine.length() + word.length() + 1 > 28) {
          infoLines.push_back(text(currentLine));
          currentLine = word;
        } else {
          if (!currentLine.empty())
            currentLine += " ";
          currentLine += word;
        }
      }
      if (!currentLine.empty()) {
        infoLines.push_back(text(currentLine));
      }
    }
  } else {
    infoLines.push_back(text("Look Mode: (press x)") | dim);
  }

  // Fill remaining space
  while (infoLines.size() < 4) {
    infoLines.push_back(text(""));
  }

  return window(text("Look At"), vbox(std::move(infoLines)));
}

Element FTXUIRenderer::buildMinimap(const GameState &state) {
  if (!state.map) {
    return window(text("Minimap"), text("N/A"));
  }

  // Simple minimap - scale map to fit
  int mapW = 60; // TODO: get from map
  int mapH = 40;
  int minimapW = 28;
  int minimapH = 8;

  int scaleX = std::max(1, mapW / minimapW);
  int scaleY = std::max(1, mapH / minimapH);

  std::vector<std::string> minimapLines;

  for (int y = 0; y < minimapH; ++y) {
    std::string line;
    for (int x = 0; x < minimapW; ++x) {
      int worldX = x * scaleX;
      int worldY = y * scaleY;

      // Check if player is in this cell
      if (state.player && state.player->getPosition().x / scaleX == x &&
          state.player->getPosition().y / scaleY == y) {
        line += '@';
        continue;
      }

      // Check if any tile in region is discovered
      bool discovered = false;
      for (const auto &tile : state.discoveredTiles) {
        if (tile.x / scaleX == x && tile.y / scaleY == y) {
          discovered = true;
          break;
        }
      }

      line += discovered ? '.' : ' ';
    }
    minimapLines.push_back(line);
  }

  std::vector<Element> lineElements;
  for (const auto &line : minimapLines) {
    lineElements.push_back(text(line));
  }

  return window(text("Minimap"), vbox(std::move(lineElements)));
}

Element FTXUIRenderer::buildStatsBar(const GameState &state) {
  if (!state.player) {
    return text("Player: N/A");
  }

  int currentHP = state.player->getProperty("hp");
  int maxHP = state.player->getProperty("max_hp");
  std::string hpBar = buildHPBar(currentHP, maxHP, 10);

  std::ostringstream oss;
  oss << " " << state.player->getName() << " HP:" << hpBar << " " << currentHP
      << "/" << maxHP;

  return hbox({text(oss.str()), filler(), text("? for help") | dim}) |
         bgcolor(Color::GrayDark);
}

char FTXUIRenderer::tileToChar(world::Tile tile) const {
  switch (tile) {
  case world::Tile::OpenGround:
    return '.';
  case world::Tile::SolidRock:
    return '#';
    /*  case world::Tile::DoorClosed:
        return '+'; // Legacy - doors are now Features, but keep for
      compatibility case world::Tile::DoorOpen: return '\''; // Legacy - doors
      are now Features case world::Tile::StairsDown: return '>'; // Legacy -
      stairs are now Features case world::Tile::StairsUp: return '<'; // Legacy
      - stairs are now Features */
  default:
    return '?';
  }
}

std::string FTXUIRenderer::buildHPBar(int current, int max, int width) const {
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

} // namespace renderers
