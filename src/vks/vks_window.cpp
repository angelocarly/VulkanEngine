#include "vks_window.h"

// std
#include <stdexcept>

namespace vks
{

    VksWindow::VksWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name}
    {
        initWindow();
    }

    VksWindow::~VksWindow()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void VksWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<VksWindow*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    void VksWindow::initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }


    void VksWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }
    }

}  // namespace vks