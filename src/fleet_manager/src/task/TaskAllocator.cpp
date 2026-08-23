#include "TaskAllocator.h"

fms::TaskAllocator::TaskAllocator(const std::unordered_map<std::string, std::shared_ptr<fms::Agent>> &agents) //
    : agents_(agents)
{
}

void fms::TaskAllocator::allocate(const fms::Task &task) const
{
  std::string best_agent_id;
  double max_score = std::numeric_limits<double>::max();

  for (const auto &[id, agent] : agents_)
  {
    if (!agent->available())
      continue;

    const auto &state = agent->state();

    // Reject agents with low battery or in error state.
    if (state.battery_percentage < 20.0 || state.status == fms::AgentStatus::ERROR)
      continue;

    // Calculate a score based on the distance to the task's source.
    double distance = std::hypot(agent->state().battery_percentage - task.source_pose.x, agent->state().battery_percentage - task.source_pose.y);
    if (distance < max_score)
    {
      max_score = distance;
      best_agent_id = id;
    }
  }
}