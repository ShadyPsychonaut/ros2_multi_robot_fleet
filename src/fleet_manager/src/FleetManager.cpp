#include "FleetManager.h"

fms::FleetManager::FleetManager() //
    : Node("fleet_manager")
{
  RCLCPP_INFO(get_logger(), "Fleet Manager started.");
  timer_ = create_wall_timer(5s, std::bind(&FleetManager::logFleetState, this));
}

void fms::FleetManager::init()
{
  auto self = shared_from_this();
  // Create agents.
  agents_["1"] = std::make_shared<fms::Agent>(self, "1", "/robot1");
  agents_["2"] = std::make_shared<fms::Agent>(self, "2", "/robot2");
  agents_["3"] = std::make_shared<fms::Agent>(self, "3", "/robot3");

  RCLCPP_INFO(get_logger(), "Fleet Manager initialized.");

  for (const auto &[id, agent] : agents_)
  {
    RCLCPP_INFO(get_logger(), "Agent %s registered.", id.c_str());
  }
}

void fms::FleetManager::logFleetState()
{
  RCLCPP_INFO(get_logger(), "Logging fleet state:");
  for (const auto &[id, agent] : agents_)
  {
    const auto &state = agent->state();
    RCLCPP_INFO(get_logger(), "Agent %s: Status: %s, Battery: %.2f%%", id.c_str(), fms::to_string(state.status).c_str(), state.battery_percentage);
  }
}

void fms::FleetManager::allocateTask()
{
  if (task_allocated_)
  {
    RCLCPP_INFO(get_logger(), "Task already allocated.");
    return;
  }

  struct TestGoal
  {
    std::string agent_id;
    double x;
    double y;
  };

  const std::vector<TestGoal> goals = {
      {"1", -1.0, 0.0},
      {"2", 1.0, 4.0},
      {"3", 0.0, -1.0},
  };

  for (const auto &goal : goals)
  {
    auto it = agents_.find(goal.agent_id);
    if (it == agents_.end())
    {
      RCLCPP_ERROR(get_logger(), "Agent %s not found.", goal.agent_id.c_str());
      continue;
    }
    if (!it->second->available())
    {
      RCLCPP_WARN(get_logger(), "Agent %s is not available.", goal.agent_id.c_str());
      continue;
    }

    RCLCPP_INFO(get_logger(), "Dispatching Agent %s to (%.2f, %.2f).", goal.agent_id.c_str(), goal.x, goal.y);
    it->second->navigateTo(goal.x, goal.y);
  }

  task_allocated_ = true;
}