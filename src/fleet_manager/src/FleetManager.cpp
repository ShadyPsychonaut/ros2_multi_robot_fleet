#include "FleetManager.h"

fms::FleetManager::FleetManager() //
    : Node("fleet_manager")
{
  RCLCPP_INFO(get_logger(), "Fleet Manager started.");

  auto self = shared_from_this();

  // Create robots.
  robots_["robot1"] = std::make_shared<fms::Robot>(self, "robot1");

  RCLCPP_INFO(get_logger(), "Registered robot1.");

  timer_ = create_wall_timer(3s, std::bind(&FleetManager::allocateTask, this));
}

void fms::FleetManager::allocateTask()
{
  if (task_allocated_)
  {
    RCLCPP_INFO(get_logger(), "Task already allocated.");
    return;
  }

  const auto &agent = robots_.at("robot1");
  if (!agent->available())
    return;

  // Create a test task.
  fms::Task task;
  task.id = "1";
  task.source = "A";
  task.destination = "B";
  task.priority = 1;

  RCLCPP_INFO(get_logger(), "Dispatching Task %s: %s -> %s", task.id.c_str(), task.source.c_str(), task.destination.c_str());

  task.status = fms::TaskStatus::ASSIGNED;

  if (agent->navigateTo(1.0, 0.0))
  {
    task.status = fms::TaskStatus::IN_PROGRESS;
    task_allocated_ = true;
    RCLCPP_INFO(get_logger(), "Task %s assigned to robot %s.", task.id.c_str(), agent->id().c_str());
  }
  else
  {
    task.status = fms::TaskStatus::FAILED;
    RCLCPP_ERROR(get_logger(), "Failed to assign Task %s", task.id.c_str());
  }
}