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
  if (test_tasks_created_)
    return;

  task_queue_.push(fms::Task{"1", "stationA", "stationB", {-1.0, 0.0}, {0.5, 0.0}, 1, fms::TaskStatus::ACCEPTED, ""});
  task_queue_.push(fms::Task{"2", "stationC", "stationD", {0.0, 1.0}, {1.0, 0.5}, 5, fms::TaskStatus::ACCEPTED, ""});
  task_queue_.push(fms::Task{"3", "stationE", "stationF", {1.0, -0.5}, {-0.5, -1.0}, 3, fms::TaskStatus::ACCEPTED, ""});

  test_tasks_created_ = true;
  RCLCPP_INFO(get_logger(), "Test tasks enqueued.");
}

void fms::FleetManager::dispatchTasks()
{
  while (!task_queue_.empty())
  {
    auto &task = task_queue_.front();
    const auto agent_id = allocator_->allocate(task);

    if (agent_id.empty())
    {
      RCLCPP_WARN(get_logger(), "No available agent for task %s. Retrying later.", task.id.c_str());
      break;
    }

    auto &agent = agents_[agent_id];
    RCLCPP_INFO(get_logger(), "Dispatching task %s to agent %s.", task.id.c_str(), agent_id.c_str());
    task.status = fms::TaskStatus::ASSIGNED;

    if (agent->navigateTo(task.source_pose.x, task.source_pose.y))
    {
      RCLCPP_INFO(get_logger(), "Task %s started by agent %s.", task.id.c_str(), agent_id.c_str());
      task.assigned_agent_id = agent_id;
      task.status = fms::TaskStatus::IN_PROGRESS;
      task_queue_.pop();
    }
    else
    {
      RCLCPP_ERROR(get_logger(), "Failed to send navigation goal for task %s to agent %s.", task.id.c_str(), agent_id.c_str());
      break;
    }
  }
}

void fms::FleetManager::logFleetState()
{
  RCLCPP_INFO(get_logger(), "Logging fleet state:");
  for (const auto &[id, agent] : agents_)
  {
    const auto &state = agent->state();
    RCLCPP_INFO(get_logger(), "Agent %s: Status: %s, Battery: %.2f%%, Pose: (%.2f, %.2f)", id.c_str(), fms::to_string(state.status).c_str(),
                state.battery_percentage, agent->state().pose.x, agent->state().pose.y);
  }
}