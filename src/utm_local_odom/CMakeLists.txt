cmake_minimum_required(VERSION 3.8)
project(utm_local_odom)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)

add_executable(utm_local_odom_node src/utm_local_odom_node.cpp)
target_include_directories(utm_local_odom_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
ament_target_dependencies(utm_local_odom_node rclcpp sensor_msgs nav_msgs)

install(TARGETS utm_local_odom_node
  DESTINATION lib/${PROJECT_NAME})

ament_package()