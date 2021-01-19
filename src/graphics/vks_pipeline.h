#ifndef VULKANENGINE_VKS_PIPELINE_H
#define VULKANENGINE_VKS_PIPELINE_H

#include <vector>
#include <string>
#include "vks_device.h"
#include "vks_swap_chain.h"

namespace vks
{

    class VksPipeline
    {
    public:
        VksPipeline(VksDevice &device, vks::VksSwapChain &swapChain);

        ~VksPipeline();


    private:
        VksDevice &device;
        VksSwapChain &swapchain;

        static std::vector<char> readFile(const std::__cxx11::basic_string<char> &filename);

        void createGraphicsPipeline();

        VkShaderModule_T *createShaderModule(const std::vector<char> &code);

    };

}

#endif //VULKANENGINE_VKS_PIPELINE_H
