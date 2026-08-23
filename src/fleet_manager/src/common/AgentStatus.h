#pragma once

#include "fleet_manager/Core.h"

namespace fms
{

enum class AgentStatus
{
  IDLE,
  RUNNING,
  FAILED,
  ERROR
};

std::string to_string(const AgentStatus &status);

} // namespace fms