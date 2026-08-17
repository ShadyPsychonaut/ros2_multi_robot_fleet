#include "Robot.h"

fms::Robot::Robot(const rclcpp::Node::SharedPtr &node, const std::string &robot_id) //
    : node_(node), robot_id_(robot_id)
{
  nav_client_ = rclcpp_action::create_client<NavigateToPose>(node_, "/navigate_to_pose");
}

bool fms::Robot::navigateTo(const double &x, const double &y)
{
  if (!is_available_)
  {
    RCLCPP_WARN(node_->get_logger(), "Robot %s is not available.", robot_id_.c_str());
    return false;
  }
  if (!nav_client_->wait_for_action_server(2s))
  {
    RCLCPP_ERROR(node_->get_logger(), "Nav2 action server unavailable for robot %s.", robot_id_.c_str());
    return false;
  }

  // Pass the goal to nav2.
  auto goal = NavigateToPose::Goal();

  goal.pose.header.frame_id = "map";
  goal.pose.header.stamp = node_->get_clock()->now();

  goal.pose.pose.position.x = x;
  goal.pose.pose.position.y = y;
  goal.pose.pose.orientation.w = 1.0;

  RCLCPP_INFO(node_->get_logger(), "Robot %s navigating to (%.2f, %.2f).", robot_id_.c_str(), x, y);

  rclcpp_action::Client<NavigateToPose>::SendGoalOptions options;
  options.goal_response_callback = std::bind(&fms::Robot::goalResponseCallback, this, std::placeholders::_1);
  options.feedback_callback = std::bind(&fms::Robot::feedbackCallback, this, std::placeholders::_1, std::placeholders::_2);
  options.result_callback = std::bind(&fms::Robot::resultCallback, this, std::placeholders::_1);

  nav_client_->async_send_goal(goal, options);
  return true;
}

bool fms::Robot::available() const
{
  return is_available_;
}

const std::string &fms::Robot::id() const
{
  return robot_id_;
}

void fms::Robot::goalResponseCallback(const GoalHandle::SharedPtr &goal_handle)
{
  if (!goal_handle)
  {
    is_available_ = true;

    RCLCPP_ERROR(node_->get_logger(), "Robot %s: navigation goal rejected.", robot_id_.c_str());

    return;
  }

  RCLCPP_INFO(node_->get_logger(), "Robot %s: navigation goal accepted.", robot_id_.c_str());
}

void fms::Robot::feedbackCallback(GoalHandle::SharedPtr, const std::shared_ptr<const NavigateToPose::Feedback> feedback)
{
  RCLCPP_INFO_THROTTLE(node_->get_logger(), *node_->get_clock(), 3000, "Robot %s: %.2f m remaining.", robot_id_.c_str(),
                       feedback->distance_remaining);
}

void fms::Robot::resultCallback(const GoalHandle::WrappedResult &result)
{
  is_available_ = true;

  switch (result.code)
  {
  case rclcpp_action::ResultCode::SUCCEEDED:
    RCLCPP_INFO(node_->get_logger(), "Robot %s: navigation succeeded.", robot_id_.c_str());
    break;

  case rclcpp_action::ResultCode::ABORTED:
    RCLCPP_ERROR(node_->get_logger(), "Robot %s: navigation aborted.", robot_id_.c_str());
    break;

  case rclcpp_action::ResultCode::CANCELED:
    RCLCPP_WARN(node_->get_logger(), "Robot %s: navigation canceled.", robot_id_.c_str());
    break;

  default:
    RCLCPP_ERROR(node_->get_logger(), "Robot %s: unknown navigation result.", robot_id_.c_str());
    break;
  }
}