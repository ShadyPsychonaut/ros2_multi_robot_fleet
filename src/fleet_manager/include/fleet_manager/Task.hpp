#pragma once

#include <cstdint>
#include <string>

namespace fms
{

enum class TaskStatus
{
  ACCEPTED,
  ASSIGNED,
  IN_PROGRESS,
  COMPLETED,
  FAILED
};

struct Task
{
  std::string id;
  std::string source;
  std::string destination;
  int priority{4};
  TaskStatus status{TaskStatus::ACCEPTED};
};

} // namespace fms