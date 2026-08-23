#include "FleetManager.h"

fms::FleetManager::FleetManager() //
    : Node("fleet_manager")
{
  RCLCPP_INFO(get_logger(), "Fleet Manager started.");
  dispatch_timer_ = create_wall_timer(2s, std::bind(&FleetManager::dispatchTasks, this));
  log_state_timer_ = create_wall_timer(5s, std::bind(&FleetManager::logFleetState, this));
}

void fms::FleetManager::init()
{
  auto self = shared_from_this();
  // Create agents.
  agents_["1"] = std::make_shared<fms::Agent>(self, "1", "/robot1");
  agents_["2"] = std::make_shared<fms::Agent>(self, "2", "/robot2");
  agents_["3"] = std::make_shared<fms::Agent>(self, "3", "/robot3");

  allocator_ = std::make_unique<fms::TaskAllocator>(agents_);

  RCLCPP_INFO(get_logger(), "Fleet Manager initialized.");

  for (const auto &[id, agent] : agents_)
  {
    RCLCPP_INFO(get_logger(), "Agent %s registered.", id.c_str());
  }

  enqueTasks();
}

void fms::FleetManager::enqueTasks()
{
}

void fms::FleetManager::dispatchTasks()
{
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