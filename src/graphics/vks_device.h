#include "vks_window.h"
#include <vector>
#include <optional>

namespace vks
{

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;

        bool isComplete() {
            return graphicsFamily.has_value();
        }
    };

    class VksDevice
    {
    public:
        VksDevice(VksWindow &window);
        ~VksDevice();
        void createInstance();

    private:
        VksWindow &window;
        VkInstance instance;

        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        VkDevice device;
        VkQueue graphicsQueue;

        // ####################### Validation / debug ########################
        const std::vector<const char *> validationLayers = {
                "VK_LAYER_KHRONOS_validation"
        };

        std::vector<const char *> getRequiredExtensions();
        bool checkValidationLayerSupport();
        VkResult
        CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                     const VkAllocationCallbacks *pAllocator,
                                     VkDebugUtilsMessengerEXT *pDebugMessenger);
        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                           const VkAllocationCallbacks *pAllocator);
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
        void setupDebugMessenger();

        bool isDeviceSuitable(VkPhysicalDevice device);

        void pickPhysicalDevice();

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

        void createLogicalDevice();
    };

}

