#ifndef VULKANENGINE_VKS_DEVICE_H
#define VULKANENGINE_VKS_DEVICE_H

#include "vks_window.h"
#include <vector>
#include <optional>

namespace vks
{

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    class VksDevice
    {
    public:
        VksDevice(VksWindow &window);
        ~VksDevice();

        SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(physicalDevice); }
        VkPhysicalDevice getPhysicalDevice() { return physicalDevice; }
        VkDevice getVkDevice() { return device; }
        VkSurfaceKHR getSurface() { return surface; }
        QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(physicalDevice); };

    private:

        VksWindow &window;
        VkInstance instance;

        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        VkDevice device;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        VkSurfaceKHR surface;

        const std::vector<const char *> validationLayers = {
                "VK_LAYER_KHRONOS_validation"
        };
        const std::vector<const char *> deviceExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        // Instancing
        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();

        //

        // ####################### helper ########################
        std::vector<const char *> getRequiredExtensions();
        bool checkValidationLayerSupport();
        VkResult
        CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                     const VkAllocationCallbacks *pAllocator,
                                     VkDebugUtilsMessengerEXT *pDebugMessenger);
        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                           const VkAllocationCallbacks *pAllocator);
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
        bool isDeviceSuitable(VkPhysicalDevice device);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);

        // Swapchain
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    };

}

#endif //VULKANENGINE_VKS_DEVICE_H
