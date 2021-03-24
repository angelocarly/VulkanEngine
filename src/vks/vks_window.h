#ifndef VULKANENGINE_VKS_WINDOW_H
#define VULKANENGINE_VKS_WINDOW_H

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

        GLFWwindow *getWindow()
        { return window; }

        int getWidth()
        { return width; }

        int getHeight()
        { return height; }

        bool framebufferResized = false;
        bool pollFrameBufferResized()
        {
            if (framebufferResized)
            {
                framebufferResized = false;
                return true;
            }
            return false;
        }

        VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }

    private:
        void initWindow();

        const int width;
        const int height;

        std::string windowName;
        GLFWwindow *window;

        static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
    };
}

#endif //VULKANENGINE_VKS_SWAP_CHAIN_H
