#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <string>

namespace vks
{

    class VksWindow
    {
    public:
        VksWindow(int w, int h, std::string name);

        ~VksWindow();

        VksWindow(const VksWindow &) = delete;

        VksWindow &operator=(const VksWindow &) = delete;

        bool shouldClose()
        { return glfwWindowShouldClose(window); }

        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

        GLFWwindow* getWindow() { return window; }

        int getWidth()
        { return width; }

        int getHeight()
        { return height; }

    private:
        void initWindow();

        const int width;
        const int height;

        std::string windowName;
        GLFWwindow *window;
    };
}