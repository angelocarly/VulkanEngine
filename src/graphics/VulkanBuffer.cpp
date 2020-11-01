//
// Created by magnias on 01/11/20.
//

#include "VulkanBuffer.h"

vks::Buffer::~Buffer()
{
    vkDestroyBuffer(device, buffer, nullptr);
    vkFreeMemory(device, memory, nullptr);
}

void vks::Buffer::cleanup() {
    vkDestroyBuffer(device, buffer, nullptr);
    vkFreeMemory(device, memory, nullptr);
}
