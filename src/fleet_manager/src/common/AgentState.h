#pragma once

#include "AgentStatus.h"
#include "fleet_manager/Core.h"

namespace fms
{
struct AgentState
{
  AgentStatus status{AgentStatus::IDLE};
  double battery_percentage{100.0};

  // Pose.
  Pose pose;
};

} // namespace fms