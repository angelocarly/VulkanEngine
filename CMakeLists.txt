cmake_minimum_required(VERSION 3.7)
project(VulkanEngine VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 17)

# Add glfw
set(GLFW_SOURCE_DIR lib/GLFW)
add_subdirectory(${GLFW_SOURCE_DIR})
include_directories(${GLFW_SOURCE_DIR}/include)

find_package(Vulkan REQUIRED)

# Add vulkan
# set(VULKAN_SOURCE_DIR lib/GLFW)
# add_subdirectory(${VULKAN_SOURCE_DIR})
# include_directories(${VULKAN_SOURCE_DIR}/include)

add_executable(VulkanEngine src/main.cpp)

target_link_libraries(VulkanEngine Vulkan::Vulkan)
target_link_libraries(VulkanEngine glfw ${GLFW_LIBRARIES})