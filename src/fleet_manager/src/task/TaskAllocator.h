#pragma once

#include <limits>

#include "Agent.h"
#include "fleet_manager/Task.hpp"

namespace fms
{
//////////////////////////////////////////////////////////////////////////////
// A simple nearest task allocator that assigns tasks to the nearest
// available agent based on their current state.
class TaskAllocator
{
public:
  explicit TaskAllocator(const std::unordered_map<std::string, std::shared_ptr<fms::Agent>> &agents);

  std::string allocate(const fms::Task &task) const;

private:
  const std::unordered_map<std::string, std::shared_ptr<fms::Agent>> &agents_;
};

} // namespace fms