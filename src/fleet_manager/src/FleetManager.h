// Here goes something
#pragma once

#include <unordered_map>

#include "Robot.h"
#include "fleet_manager/Core.h"
#include "fleet_manager/Task.hpp"

namespace fms
{
using namespace std::chrono_literals;

class FleetManager : public rclcpp::Node
{
public:
  FleetManager();

private:
  void allocateTask();

private:
  std::unordered_map<std::string, std::shared_ptr<fms::Robot>> robots_;
  rclcpp::TimerBase::SharedPtr timer_;
  bool task_allocated_{false};
};

} // namespace fms