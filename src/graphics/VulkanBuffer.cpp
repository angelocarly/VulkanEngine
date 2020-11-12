//
// Created by magnias on 01/11/20.
//

#include "VulkanBuffer.h"

void vks::Buffer::destroy() {
    vkDestroyBuffer(device, buffer, nullptr);
    vkFreeMemory(device, memory, nullptr);
}
