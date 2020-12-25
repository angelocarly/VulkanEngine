# Config taken from https://github.com/aaronmjacobs/InitGL

# Project definition
cmake_minimum_required(VERSION 3.1)
project(VulkanEngine VERSION 0.1.0)

# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(SOURCES "${SRC_DIR}/main.cpp" src/game/Entity.cpp src/game/Entity.h lib/stb/stb_image.h src/graphics/VulkanDevice.h)

# Executable definition and properties
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

# Shaders
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${SRC_DIR}/shaders
        ${CMAKE_CURRENT_BINARY_DIR}/shaders)

# Vulkan
find_package(Vulkan REQUIRED)
target_link_libraries(VulkanEngine Vulkan::Vulkan)

# GLFW
find_package(glfw3 3.3 REQUIRED)
target_link_libraries(VulkanEngine glfw)
#set(GLFW_DIR "${LIB_DIR}/glfw")
#set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs") # Disable GLFW standard programs from being built
#set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
#set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
#set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
#add_subdirectory("${GLFW_DIR}")
#target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
#target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
#target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# glad
set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")

# glm
find_package(glm REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})
link_directories(${GLM_LIBRARY_DIRS})
add_definitions(${GLM_DEFINITIONS})

# timyobj
find_package(tinyobjloader REQUIRED)
include_directories(${TINYOBJLOADER_INCLUDE_DIRS})
link_directories(${TINYOBJELOADER_LIBRARY_DIRS})
add_definitions(${TINYOBJLOADER_DEFINITIONS})

# stb
set(STB_DIR "${LIB_DIR}/stb")
#add_library("glad" "${GLAD_DIR}/src/glad.c")
include_directories(${STB_DIR})
#link_directories(${STB_LIBRARY_DIRS})
#add_definitions(${STB_DEFINITIONS})
