//
// Created by magnias on 20/10/20.
//

#include "Window.h"
#include <GLFW/glfw3.h>

Window::Window(int width, int height)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window_handle = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window_handle, this);
    glfwSetFramebufferSizeCallback(window_handle, framebufferResizeCallback);

}

void Window::framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    auto app = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(window_handle);
}

void Window::pollEvents()
{
    glfwPollEvents();
}

void Window::destroy()
{
    glfwDestroyWindow(window_handle);
    glfwTerminate();
}

GLFWwindow *Window::getGLFWwindow()
{
    return window_handle;
}

