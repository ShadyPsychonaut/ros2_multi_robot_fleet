#pragma once

#include "fleet_manager/Core.h"
#include "fleet_manager/Pose.h"

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

  Pose source_pose;
  Pose destination_pose;

  int priority{4};
  TaskStatus status{TaskStatus::ACCEPTED};

  std::string assigned_agent_id;
};

} // namespace fms