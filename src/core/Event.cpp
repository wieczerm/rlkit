#include "core/Event.hpp"

namespace core {

EventPriority getPriority(const Event &e) {
  return std::visit(
      [](const auto &evt) -> EventPriority {
        return std::remove_cvref_t<decltype(evt)>::priority;
      },
      e);
}

} // namespace core
