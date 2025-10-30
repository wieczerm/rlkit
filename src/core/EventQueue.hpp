
#pragma once
#include "core/Event.hpp"
#include <functional>
#include <vector>

namespace core {

// Event queue for deferred event processing
// Events are queued during gameplay and processed in batches
// Processing order: sorted by priority, then FIFO within same priority
class EventQueue {
public:
  EventQueue() = default;

  // Non-copyable - queue is transient, lives in Game class
  EventQueue(const EventQueue &) = delete;
  EventQueue &operator=(const EventQueue &) = delete;

  // Push event to queue for later processing
  void push(Event e);

  // Process all queued events with callback
  // Events are sorted by priority before processing
  // Callback receives const Event& for each event
  void process(std::function<void(const Event &)> callback);

  // Clear queue without processing (use with caution)
  void clear();

  // Query state
  bool empty() const;
  std::size_t size() const;

private:
  std::vector<Event> events_;
};

} // namespace core
