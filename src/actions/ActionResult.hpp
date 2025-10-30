#pragma once
#include <string>

namespace actions {

enum class ActionStatus { Success, Failure, Invalid };

struct ActionResult {
  ActionStatus status;
  std::string message;
  int energy_cost;

  static ActionResult success(const std::string &msg, int cost = 100) {
    return {ActionStatus::Success, msg, cost};
  }

  static ActionResult failure(const std::string &msg) {
    return {ActionStatus::Failure, msg, 0};
  }

  static ActionResult invalid(const std::string &msg) {
    return {ActionStatus::Invalid, msg, 0};
  }
};

} // namespace actions
