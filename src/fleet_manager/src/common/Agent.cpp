#include "Agent.h"

#include "tf2/exceptions.h"
#include "tf2_ros/qos.hpp"

fms::Agent::Agent(const rclcpp::Node::SharedPtr &node, const std::string &id, const std::string &ns) //
    : node_(node), agent_id_(id), namespace_(ns)
{
  nav_client_ = rclcpp_action::create_client<NavigateToPose>(node_, ns + "/navigate_to_pose");
  // Transform listener.
  tf_buffer_ = std::make_unique<tf2_ros::Buffer>(node_->get_clock());
  tf_subscription_ = node_->create_subscription<tf2_msgs::msg::TFMessage>(
      ns + "/tf", tf2_ros::DynamicListenerQoS(), [this](const tf2_msgs::msg::TFMessage::SharedPtr msg) { transformCallback(msg, false); });
  tf_static_subscription_ = node_->create_subscription<tf2_msgs::msg::TFMessage>(
      ns + "/tf_static", tf2_ros::StaticListenerQoS(), [this](const tf2_msgs::msg::TFMessage::SharedPtr msg) { transformCallback(msg, true); });
  update_timer_ = node_->create_wall_timer(100ms, std::bind(&fms::Agent::update, this));
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
  state_.status = AgentStatus::RUNNING;

  rclcpp_action::Client<NavigateToPose>::SendGoalOptions options;
  options.goal_response_callback = std::bind(&fms::Agent::goalResponseCallback, this, std::placeholders::_1);
  options.feedback_callback = std::bind(&fms::Agent::feedbackCallback, this, std::placeholders::_1, std::placeholders::_2);
  options.result_callback = std::bind(&fms::Agent::resultCallback, this, std::placeholders::_1);

  nav_client_->async_send_goal(goal, options);
  return true;
}

double fms::Agent::distanceTo(const double &x, const double &y) const
{
  return Utils::norm2D(state_.pose.x - x, state_.pose.y - y);
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

void fms::Agent::update()
{
  try
  {
    const auto transform = tf_buffer_->lookupTransform("odom", "base_footprint", tf2::TimePointZero);
    state_.pose.x = transform.transform.translation.x;
    state_.pose.y = transform.transform.translation.y;
  }
  catch (const tf2::TransformException &e)
  {
    RCLCPP_WARN_THROTTLE(node_->get_logger(), *node_->get_clock(), 5000, "Agent %s: unable to get pose: %s", agent_id_.c_str(), e.what());
  }
}

void fms::Agent::transformCallback(const tf2_msgs::msg::TFMessage::SharedPtr msg, bool is_static)
{
  for (const auto &tf : msg->transforms)
  {
    try
    {
      tf_buffer_->setTransform(tf, agent_id_, is_static);
    }
    catch (const tf2::TransformException &e)
    {
      RCLCPP_WARN(node_->get_logger(), "Agent %s: failed to store TF %s -> %s: %s", agent_id_.c_str(), tf.header.frame_id.c_str(),
                  tf.child_frame_id.c_str(), e.what());
    }
  }
}

void fms::Agent::goalResponseCallback(const GoalHandle::SharedPtr &goal_handle)
{
  if (!goal_handle)
  {
    state_.status = AgentStatus::FAILED;
    RCLCPP_ERROR(node_->get_logger(), "Agent %s: navigation goal rejected.", agent_id_.c_str());
    return;
  }

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