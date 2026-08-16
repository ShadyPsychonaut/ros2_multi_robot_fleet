// Here goes something
#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "nav2_msgs/action/navigate_to_pose.hpp"

using namespace std::chrono_literals;

class FleetManager : public rclcpp::Node
{
    using NavigateToPose = nav2_msgs::action::NavigateToPose;
    using GoalHandleNavigate = rclcpp_action::ClientGoalHandle<NavigateToPose>;

public:
    FleetManager();

private:
    void heartbeat();
    //
    void sendTestGoal();
    void goalResponseCallback(const GoalHandleNavigate::SharedPtr goal_handle);
    void feedbackCallback(GoalHandleNavigate::SharedPtr, const std::shared_ptr<const NavigateToPose::Feedback> feedback);
    void resultCallback(const GoalHandleNavigate::WrappedResult & result);

private:
    rclcpp::TimerBase::SharedPtr timer_;
    //
    rclcpp_action::Client<NavigateToPose>::SharedPtr nav_client_;
};