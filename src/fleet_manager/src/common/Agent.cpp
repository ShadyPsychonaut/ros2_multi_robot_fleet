#include "Agent.h"

fms::Agent::Agent(const rclcpp::Node::SharedPtr &node, const std::string &id, const std::string &ns) //
    : node_(node), agent_id_(id), namespace_(ns)
{
  nav_client_ = rclcpp_action::create_client<NavigateToPose>(node_, ns + "/navigate_to_pose");
}

bool fms::Agent::navigateTo(const double &x, const double &y)
{
  if (!available())
  {
    RCLCPP_WARN(node_->get_logger(), "Agent %s is not available.", agent_id_.c_str());
    return false;
  }

  RCLCPP_INFO(node_->get_logger(), "Agent %s waiting for action server: %s/navigate_to_pose", agent_id_.c_str(), namespace_.c_str());

  if (!nav_client_->wait_for_action_server(2s))
  {
    RCLCPP_ERROR(node_->get_logger(), "Nav2 action server unavailable for Agent %s.", agent_id_.c_str());
    return false;
  }

  // Pass the goal to nav2.
  auto goal = NavigateToPose::Goal();

  goal.pose.header.frame_id = "map";
  goal.pose.header.stamp = node_->get_clock()->now();

  goal.pose.pose.position.x = x;
  goal.pose.pose.position.y = y;
  goal.pose.pose.orientation.w = 1.0;

  RCLCPP_INFO(node_->get_logger(), "Agent %s navigating to (%.2f, %.2f).", agent_id_.c_str(), x, y);

  rclcpp_action::Client<NavigateToPose>::SendGoalOptions options;
  options.goal_response_callback = std::bind(&fms::Agent::goalResponseCallback, this, std::placeholders::_1);
  options.feedback_callback = std::bind(&fms::Agent::feedbackCallback, this, std::placeholders::_1, std::placeholders::_2);
  options.result_callback = std::bind(&fms::Agent::resultCallback, this, std::placeholders::_1);

  nav_client_->async_send_goal(goal, options);
  return true;
}

bool fms::Agent::available() const
{
  return state_.status == AgentStatus::IDLE;
}

const std::string &fms::Agent::id() const
{
  return agent_id_;
}

const fms::AgentState &fms::Agent::state() const
{
  return state_;
}

void fms::Agent::goalResponseCallback(const GoalHandle::SharedPtr &goal_handle)
{
  if (!goal_handle)
  {
    state_.status = AgentStatus::FAILED;
    RCLCPP_ERROR(node_->get_logger(), "Agent %s: navigation goal rejected.", agent_id_.c_str());
    return;
  }

  state_.status = AgentStatus::RUNNING;
  RCLCPP_INFO(node_->get_logger(), "Agent %s: navigation goal accepted.", agent_id_.c_str());
}

void fms::Agent::feedbackCallback(GoalHandle::SharedPtr, const std::shared_ptr<const NavigateToPose::Feedback> feedback)
{
  RCLCPP_INFO_THROTTLE(node_->get_logger(), *node_->get_clock(), 3000, "Agent %s: %.2f m remaining.", agent_id_.c_str(),
                       feedback->distance_remaining);
}

void fms::Agent::resultCallback(const GoalHandle::WrappedResult &result)
{
  switch (result.code)
  {
  case rclcpp_action::ResultCode::SUCCEEDED:
    state_.status = AgentStatus::IDLE;
    RCLCPP_INFO(node_->get_logger(), "Agent %s: navigation succeeded.", agent_id_.c_str());
    break;

  case rclcpp_action::ResultCode::ABORTED:
    state_.status = AgentStatus::FAILED;
    RCLCPP_ERROR(node_->get_logger(), "Agent %s: navigation aborted.", agent_id_.c_str());
    break;

  case rclcpp_action::ResultCode::CANCELED:
    state_.status = AgentStatus::IDLE;
    RCLCPP_WARN(node_->get_logger(), "Agent %s: navigation canceled.", agent_id_.c_str());
    break;

  default:
    state_.status = AgentStatus::FAILED;
    RCLCPP_ERROR(node_->get_logger(), "Agent %s: unknown navigation result.", agent_id_.c_str());
    break;
  }
}