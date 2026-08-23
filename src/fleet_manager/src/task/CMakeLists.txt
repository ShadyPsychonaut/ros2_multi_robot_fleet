# CMakeLists.txt for task

file(GLOB TASK_SRC CONFIGURE_DEPENDS "*.cpp")

add_library(task SHARED
  ${TASK_SRC}
)

target_link_libraries(task common)

ament_target_dependencies(task
  rclcpp
  rclcpp_action
  nav2_msgs
)

target_include_directories(task PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)