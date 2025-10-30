
#include "core/EventQueue.hpp"
#include <algorithm>

namespace core {

void EventQueue::push(Event e) { events_.push_back(std::move(e)); }

void EventQueue::process(std::function<void(const Event &)> callback) {
  if (events_.empty()) {
    return;
  }

  // Sort by priority (lower number = higher priority)
  // stable_sort preserves FIFO order within same priority level
  std::stable_sort(events_.begin(), events_.end(),
                   [](const Event &a, const Event &b) {
                     return getPriority(a) < getPriority(b);
                   });

  // Process each event in priority order
  for (const auto &event : events_) {
    callback(event);
  }

  // Clear queue after processing
  events_.clear();
}

void EventQueue::clear() { events_.clear(); }

bool EventQueue::empty() const { return events_.empty(); }

std::size_t EventQueue::size() const { return events_.size(); }

} // namespace core
