//
// Created by magnias on 01/11/20.
//

#include <stdexcept>
#include "VulkanDevice.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <set>
#include <cstring>
#include <GLFW/glfw3.h>


struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

//void VulkanDevice::createSurface() {
//    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create window surface!");
//    }
//}
//
//void VulkanDevice::pickPhysicalDevice() {
//    uint32_t deviceCount = 0;
//    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
//
//    if (deviceCount == 0) {
//        throw std::runtime_error("failed to find GPUs with Vulkan support!");
//    }
//
//    std::vector<VkPhysicalDevice> devices(deviceCount);
//    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
//
//    for (const auto &device : devices) {
//        if (isDeviceSuitable(device)) {
//            physicalDevice = device;
//            break;
//        }
//    }
//
//    if (physicalDevice == VK_NULL_HANDLE) {
//        throw std::runtime_error("failed to find a suitable GPU!");
//    }
//}
//
//void VulkanDevice::createLogicalDevice() {
//    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
//
//    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
//    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
//
//    float queuePriority = 1.0f;
//    for (uint32_t queueFamily : uniqueQueueFamilies) {
//        VkDeviceQueueCreateInfo queueCreateInfo{};
//        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
//        queueCreateInfo.queueFamilyIndex = queueFamily;
//        queueCreateInfo.queueCount = 1;
//        queueCreateInfo.pQueuePriorities = &queuePriority;
//        queueCreateInfos.push_back(queueCreateInfo);
//    }
//
//    VkPhysicalDeviceFeatures deviceFeatures{};
//
//    VkDeviceCreateInfo createInfo{};
//    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
//
//    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
//    createInfo.pQueueCreateInfos = queueCreateInfos.data();
//
//    createInfo.pEnabledFeatures = &deviceFeatures;
//
//    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
//    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
//
//    if (enableValidationLayers) {
//        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
//        createInfo.ppEnabledLayerNames = validationLayers.data();
//    } else {
//        createInfo.enabledLayerCount = 0;
//    }
//
//    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create logical device!");
//    }
//
//    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
//    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
//}
