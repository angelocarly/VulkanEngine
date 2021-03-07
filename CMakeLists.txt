# Config taken from https://github.com/aaronmjacobs/InitGL

# Project definition
cmake_minimum_required(VERSION 3.1)
project(VulkanEngine VERSION 0.1.0)

# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(SOURCES "${SRC_DIR}/main.cpp" lib/stb/stb_image.h src/graphics/VulkanInitializers.h src/game/Game.h src/app.hpp src/graphics/vks_window.cpp src/graphics/vks_window.h src/graphics/vks_device.cpp src/graphics/vks_device.h src/graphics/vks_swap_chain.cpp src/graphics/vks_swap_chain.h src/graphics/vks_pipeline.cpp src/graphics/vks_pipeline.h src/graphics/vks_framebuffer.cpp src/graphics/vks_framebuffer.h
        src/imgui/imconfig.h
        src/imgui/imgui.cpp
        src/imgui/imgui.h
        src/imgui/imgui_demo.cpp
        src/imgui/imgui_draw.cpp
        src/imgui/imgui_internal.h
        src/imgui/imgui_tables.cpp
        src/imgui/imgui_widgets.cpp
        src/imgui/imstb_rectpack.h
        src/imgui/imstb_textedit.h
        src/imgui/imstb_truetype.h
        src/graphics/vks_model.cpp src/graphics/vks_model.h src/imgui/imgui_impl_vulkan.h src/imgui/imgui_impl_vulkan.cpp src/imgui/imgui_impl_glfw.cpp src/imgui/imgui_impl_glfw.h src/app.cpp
        )

# Executable definition and properties
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)


# Shaders
if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
    set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/usr/bin/glslangValidator")
else()
    set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/usr/bin/glslangValidator")
endif()

file(GLOB_RECURSE GLSL_SOURCE_FILES
        "shaders/*.frag"
        "shaders/*.vert"
        )

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
            COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
        Shaders
        DEPENDS ${SPIRV_BINARY_FILES}
)

add_dependencies(${PROJECT_NAME} Shaders)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:VulkanEngine>/shaders/"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_BINARY_DIR}/shaders"
        "$<TARGET_FILE_DIR:VulkanEngine>/shaders"
        )


# Vulkan
find_package(Vulkan REQUIRED)
target_link_libraries(VulkanEngine Vulkan::Vulkan)

# GLFW
find_package(glfw3 3.3 REQUIRED)
target_link_libraries(${PROJECT_NAME} glfw)
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

# tinyobj
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

# spdlog
find_package(spdlog REQUIRED)
target_link_libraries(${PROJECT_NAME} spdlog::spdlog)