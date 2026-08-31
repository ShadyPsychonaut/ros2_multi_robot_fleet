#pragma once

#include "AgentState.h"
#include "fleet_manager/Core.h"

#include "nav2_msgs/action/navigate_to_pose.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "tf2/buffer_core.h"
#include "tf2_msgs/msg/tf_message.hpp"
#include "tf2_ros/buffer.h"

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
  double distanceTo(const double &x, const double &y) const;

  // Getters.
  bool available() const;
  const std::string &id() const;
  const fms::AgentState &state() const;

private:
  // Update the agent.
  void update();
  // TF2 callbacks.
  void transformCallback(const tf2_msgs::msg::TFMessage::SharedPtr msg, bool is_static);
  // nav2 callbacks.
  void goalResponseCallback(const GoalHandle::SharedPtr &goal_handle);
  void feedbackCallback(GoalHandle::SharedPtr, const std::shared_ptr<const NavigateToPose::Feedback> feedback);
  void resultCallback(const GoalHandle::WrappedResult &result);

private:
  rclcpp::Node::SharedPtr node_;
  std::string agent_id_;
  std::string namespace_{"/"};
  AgentState state_;
  rclcpp_action::Client<NavigateToPose>::SharedPtr nav_client_;

  std::unique_ptr<tf2_ros::Buffer> tf_buffer_;
  rclcpp::Subscription<tf2_msgs::msg::TFMessage>::SharedPtr tf_subscription_;
  rclcpp::Subscription<tf2_msgs::msg::TFMessage>::SharedPtr tf_static_subscription_;
  rclcpp::TimerBase::SharedPtr update_timer_;
};

} // namespace fms