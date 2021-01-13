#ifndef VULKANENGINE_VKS_PIPELINE_H
#define VULKANENGINE_VKS_PIPELINE_H

#include <vector>
#include <string>
#include "vks_device.h"

namespace vks
{

    class VksPipeline
    {
    public:
        VksPipeline();

        VksPipeline(vks::VksDevice &device);

        static std::vector<char> readFile(const std::__cxx11::basic_string<char> &filename);

        void createGraphicsPipeline();

        VkShaderModule_T *createShaderModule(const std::vector<char> &code);

        VksDevice &device;
    };

}

#endif //VULKANENGINE_VKS_PIPELINE_H
