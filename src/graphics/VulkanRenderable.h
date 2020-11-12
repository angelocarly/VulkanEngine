//
// Created by magnias on 01/11/20.
//

#include <vulkan/vulkan.h>

#ifndef VULKANENGINE_VULKANRENDERABLE_H
#define VULKANENGINE_VULKANRENDERABLE_H

#endif //VULKANENGINE_VULKANRENDERABLE_H

namespace vks
{
    struct Renderable
    {
        virtual void draw(VkCommandBuffer cmdbuffer, VkPipelineLayout pipelineLayout, int index) = 0;
    };
}