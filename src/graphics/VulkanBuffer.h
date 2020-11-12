//
// Created by magnias on 01/11/20.
//

#ifndef VULKANENGINE_VULKANBUFFER_H
#define VULKANENGINE_VULKANBUFFER_H


#include <vulkan/vulkan.h>

namespace vks
{
    struct Buffer
    {
        VkBuffer buffer;
        VkDeviceMemory memory;
        VkDevice device;

        void destroy();
    };
}


#endif //VULKANENGINE_VULKANBUFFER_H
