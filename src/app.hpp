#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <graphics/vks_window.h>
#include <graphics/vks_device.h>
#include <graphics/vks_swap_chain.h>
#include <graphics/vks_pipeline.h>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

namespace vks
{
    class App
    {
    public:
        void run()
        {
            mainLoop();
        }

    private:
        VksWindow window = VksWindow(WIDTH, HEIGHT, "test");
        VksDevice device = VksDevice(window);
        VksSwapChain swapChain = VksSwapChain(window, device);
        VksPipeline pipeline = VksPipeline(device);

        void mainLoop()
        {
            while (!window.shouldClose())
            {
                glfwPollEvents();
            }
        }

    };


}