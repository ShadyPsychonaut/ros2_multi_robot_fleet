// Here goes something
#pragma once

#include <queue>

#include "common/Agent.h"
#include "fleet_manager/Core.h"
#include "fleet_manager/Task.hpp"
#include "task/TaskAllocator.h"

namespace fms
{
using namespace std::chrono_literals;

class FleetManager : public rclcpp::Node
{
public:
  FleetManager();
  void init();

private:
  void enqueTasks();
  void dispatchTasks();
  void logFleetState();

private:
  std::unordered_map<std::string, std::shared_ptr<fms::Agent>> agents_;
  std::queue<fms::Task> task_queue_;
  std::unique_ptr<fms::TaskAllocator> allocator_;

  rclcpp::TimerBase::SharedPtr dispatch_timer_;
  rclcpp::TimerBase::SharedPtr log_state_timer_;

  bool test_tasks_created_{false};
};

} // namespace fms