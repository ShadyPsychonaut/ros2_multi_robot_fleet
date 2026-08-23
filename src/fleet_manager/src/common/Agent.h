#pragma once

#include "fleet_manager/Core.h"

#include "nav2_msgs/action/navigate_to_pose.hpp"
#include "rclcpp_action/rclcpp_action.hpp"

namespace fms
{
using namespace std::chrono_literals;

class Agent
{
public:
  using NavigateToPose = nav2_msgs::action::NavigateToPose;
  using GoalHandle = rclcpp_action::ClientGoalHandle<NavigateToPose>;

  Agent(const rclcpp::Node::SharedPtr &node, const std::string &id, const std::string &ns = "/");

  bool navigateTo(const double &x, const double &y);

  // Getters.
  bool available() const;
  const std::string &id() const;

private:
  void goalResponseCallback(const GoalHandle::SharedPtr &goal_handle);
  void feedbackCallback(GoalHandle::SharedPtr, const std::shared_ptr<const NavigateToPose::Feedback> feedback);
  void resultCallback(const GoalHandle::WrappedResult &result);

private:
  rclcpp::Node::SharedPtr node_;
  std::string agent_id_;
  std::string namespace_{"/"};
  bool is_available_{true};
  rclcpp_action::Client<NavigateToPose>::SharedPtr nav_client_;
};

} // namespace fms