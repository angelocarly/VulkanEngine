#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <graphics/vks_window.h>
#include <graphics/vks_device.h>
#include <graphics/vks_swap_chain.h>
#include <graphics/vks_pipeline.h>
#include <memory>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

namespace vks
{
    class App
    {
    public:
        void run()
        {
            createPipelineLayout();
            createPipeline();
            mainLoop();
        }

        ~App()
        {
            vkDestroyPipelineLayout(device.getVkDevice(), pipelineLayout, nullptr);
        }

    private:
        VksWindow window = VksWindow(WIDTH, HEIGHT, "test");
        VksDevice device = VksDevice(window);
        VksSwapChain swapChain = VksSwapChain(window, device);
        std::unique_ptr<VksPipeline> pipeline;
        VkPipelineLayout pipelineLayout;

        void mainLoop()
        {
            while (!window.shouldClose())
            {
                glfwPollEvents();
            }
        }

        void createPipelineLayout()
        {
            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 0;
            pipelineLayoutInfo.pSetLayouts = nullptr;
            pipelineLayoutInfo.pushConstantRangeCount = 0;
            pipelineLayoutInfo.pPushConstantRanges = nullptr;
            if (vkCreatePipelineLayout(device.getVkDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
                VK_SUCCESS) {
                throw std::runtime_error("failed to create pipeline layout!");
            }
        }

        void createPipeline()
        {
            auto pipelineConfig = VksPipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT);
            pipelineConfig.renderPass = swapChain.getRenderPass();
            pipelineConfig.pipelineLayout = pipelineLayout;
            pipeline = std::make_unique<VksPipeline>(device, swapChain, pipelineConfig);
        }

    };


}