cmake_minimum_required(VERSION 3.5)
project(speak_ros_voicevox_plugin)

find_package(ament_cmake_auto REQUIRED)
find_package(cpprestsdk REQUIRED)
ament_auto_find_build_dependencies()

add_library(${PROJECT_NAME} SHARED src/${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

ament_target_dependencies(${PROJECT_NAME}
  rclcpp
  speak_ros
  pluginlib
)

target_link_libraries(${PROJECT_NAME} cpprestsdk::cpprest)

pluginlib_export_plugin_description_file(speak_ros plugins.xml)


install(
  TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_export_libraries(
  ${PROJECT_NAME}
)
ament_export_targets(
  export_${PROJECT_NAME}
)

ament_auto_package()
