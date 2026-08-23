#include "AgentStatus.h"

std::string fms::to_string(const AgentStatus &status)
{
  switch (status)
  {
  case AgentStatus::IDLE:
    return "IDLE";
  case AgentStatus::RUNNING:
    return "RUNNING";
  case AgentStatus::FAILED:
    return "FAILED";
  case AgentStatus::ERROR:
    return "ERROR";
  default:
    return "UNKNOWN";
  }
}