cmake_minimum_required(VERSION 3.7)
project(VulkanEngine VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 17)

# Add glfw
set(GLFW_SOURCE_DIR lib/GLFW)
add_subdirectory(${GLFW_SOURCE_DIR})
include_directories(${GLFW_SOURCE_DIR}/include)

# Add Vulkan
find_package(Vulkan REQUIRED)

file(GLOB_RECURSE SRCS
    ${VulkanEngine_SOURCE_DIR}/src/*.cpp
	${VulkanEngine_SOURCE_DIR}/src/*.c
)
add_executable(VulkanEngine ${SRCS})

target_link_libraries(VulkanEngine Vulkan::Vulkan)
target_link_libraries(VulkanEngine glfw ${GLFW_LIBRARIES})