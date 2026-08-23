// Here goes something
#pragma once

#include <unordered_map>

#include "common/Agent.h"
#include "fleet_manager/Core.h"
#include "fleet_manager/Task.hpp"

namespace fms
{
using namespace std::chrono_literals;

class FleetManager : public rclcpp::Node
{
public:
  FleetManager();
  void init();

private:
  void allocateTask();

private:
  std::unordered_map<std::string, std::shared_ptr<fms::Agent>> agents_;
  rclcpp::TimerBase::SharedPtr timer_;
  bool task_allocated_{false};
};

} // namespace fms