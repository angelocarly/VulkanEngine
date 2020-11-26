//
// Created by magnias on 01/11/20.
//

#ifndef VULKANENGINE_VULKANDEVICE_H
#define VULKANENGINE_VULKANDEVICE_H


#include <vulkan/vulkan.h>
#include <msgpack/v1/adaptor/vector.hpp>

struct VulkanDevice {
public:

    void createInstance();

    void setupDebugMessenger();

    void createSurface();

    void pickPhysicalDevice();

    void createLogicalDevice();

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

private:
    VkDevice device;
    bool enableValidationLayers = true;

    bool checkValidationLayerSupport();

    std::vector<const char *> getRequiredExtensions() const;

    void createInstance(VkInstance *instance);
};


#endif //VULKANENGINE_VULKANDEVICE_H
