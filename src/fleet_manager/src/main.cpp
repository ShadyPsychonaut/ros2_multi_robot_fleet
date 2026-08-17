#include "FleetManager.h"

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto fleet_manager = std::make_shared<fms::FleetManager>();
  rclcpp::spin(fleet_manager);
  rclcpp::shutdown();
  return 0;
}