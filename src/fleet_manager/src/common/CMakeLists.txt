# CMakeLists.txt for common

file(GLOB COMMON_SRC CONFIGURE_DEPENDS "*.cpp")

add_library(common SHARED
  ${COMMON_SRC}
)

ament_target_dependencies(common
  rclcpp
  rclcpp_action
  nav2_msgs
  tf2
  tf2_ros
  tf2_msgs
)

target_include_directories(common PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)