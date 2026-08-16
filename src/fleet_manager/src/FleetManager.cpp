#include "FleetManager.h"

FleetManager::FleetManager()
    : Node("fleet_manager")
{
    RCLCPP_INFO(this->get_logger(), "Fleet Manager started");

    nav_client_ = rclcpp_action::create_client<NavigateToPose>(this, "/navigate_to_pose");
    timer_ = this->create_wall_timer(2s, std::bind(&FleetManager::sendTestGoal, this));
}

void FleetManager::heartbeat()
{
    RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 5000, "Fleet Manager alive");
}

void FleetManager::sendTestGoal()
{
    timer_->cancel();
    if (!nav_client_->wait_for_action_server(5s))
    {
        RCLCPP_ERROR(this->get_logger(), "NavigateToPose action server not available");
        return;
    }
    
    auto goal = NavigateToPose::Goal();
    goal.pose.header.frame_id = "map";
    goal.pose.header.stamp = this->get_clock()->now();

    // Destination.
    goal.pose.pose.position.x = 1.0;
    goal.pose.pose.position.y = 0.0;
    goal.pose.pose.orientation.w = 1.0;

    RCLCPP_INFO(this->get_logger(), "Sending goal : (%.2f, %.2f)", goal.pose.pose.position.x, goal.pose.pose.position.y);

    rclcpp_action::Client<NavigateToPose>::SendGoalOptions options;
    options.goal_response_callback = std::bind(&FleetManager::goalResponseCallback, this, std::placeholders::_1);
    options.feedback_callback = std::bind(&FleetManager::feedbackCallback, this, std::placeholders::_1, std::placeholders::_2);
    options.result_callback = std::bind(&FleetManager::resultCallback, this, std::placeholders::_1);

    nav_client_->async_send_goal(goal, options);
}

void FleetManager::goalResponseCallback(const GoalHandleNavigate::SharedPtr goal_handle)
{
    if (!goal_handle)
    {
        RCLCPP_ERROR(this->get_logger(), "Goal was rejected by the action server");
        return;
    }

    RCLCPP_INFO(this->get_logger(), "Goal accepted by the action server, waiting for result");
}

void FleetManager::feedbackCallback(GoalHandleNavigate::SharedPtr, const std::shared_ptr<const NavigateToPose::Feedback> feedback)
{
    RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 2000, "Received feedback: (%.2f, %.2f), Distance remaining: %.2f", feedback->current_pose.pose.position.x, feedback->current_pose.pose.position.y, feedback->distance_remaining);
}

void FleetManager::resultCallback(const GoalHandleNavigate::WrappedResult & result)
{
    switch (result.code)
    {
        case rclcpp_action::ResultCode::SUCCEEDED:
            RCLCPP_INFO(this->get_logger(), "Goal succeeded!");
            break;
        case rclcpp_action::ResultCode::ABORTED:
            RCLCPP_ERROR(this->get_logger(), "Goal was aborted");
            break;
        case rclcpp_action::ResultCode::CANCELED:
            RCLCPP_WARN(this->get_logger(), "Goal was canceled");
            break;
        default:
            RCLCPP_ERROR(this->get_logger(), "Unknown result code");
            break;
    }
}

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<FleetManager>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}