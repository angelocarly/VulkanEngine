#ifndef VULKANENGINE_VKS_DEVICE_H
#define VULKANENGINE_VKS_DEVICE_H

#include "vks_window.h"
#include <vector>
#include <optional>
#include <spdlog/logger.h>

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
		std::optional<uint32_t> computeFamily;

		bool isComplete()
		{
			return graphicsFamily.has_value() && presentFamily.has_value() && presentFamily.has_value();
		}
	};

	/*
	 * Abstraction around the vulkan instance, physical device and logical device
	 *
	 * A Vulkan instance is needed in order to use Vulkan.
	 * The physical device is a device on the system that supports Vulkan, usually your graphics card.
	 * The logical device is a software binding that allows us to interact with the physical device.
	 */
	class VksDevice
	{
	 public:
		VksDevice(VksWindow& window, bool enableValidationLayers);

		~VksDevice()
		{
		}

		void destroy();

		SwapChainSupportDetails getSwapChainSupport()
		{
			return querySwapChainSupport(physicalDevice);
		}
		VkPhysicalDevice getPhysicalDevice()
		{
			return physicalDevice;
		}
		VkDevice getVkDevice()
		{
			return device;
		}
		VkSurfaceKHR getSurface()
		{
			return surface;
		}
		VkFormat findSupportedFormat(
			const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		QueueFamilyIndices findQueueFamilies()
		{
			return findQueueFamilies(this->physicalDevice);
		}
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		VkCommandPool getCommandPool()
		{
			return commandPool;
		};
		VkQueue getGraphicsQueue()
		{
			return graphicsQueue;
		}
		VkQueue getComputeQueue()
		{
			return computeQueue;
		}
		VkQueue getPresentQueue()
		{
			return presentQueue;
		}
		VkInstance getInstance();

		void createBuffer(
			VkDeviceSize size,
			VkBufferUsageFlags usage,
			VkMemoryPropertyFlags properties,
			VkBuffer& buffer,
			VkDeviceMemory& bufferMemory);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		VkCommandBuffer beginSingleTimeCommands();

		void endSingleTimeCommands(VkCommandBuffer commandBuffer);

		VkCommandBuffer createCommandBuffer(VkCommandBufferLevel level, VkCommandPool pool, bool begin);
		VkCommandBuffer createCommandBuffer(VkCommandBufferLevel level, bool begin);
		uint32_t
		getMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound = nullptr) const;

		void createImageWithInfo(
			const VkImageCreateInfo& imageInfo,
			VkMemoryPropertyFlags properties,
			VkImage& image,
			VkDeviceMemory& imageMemory);
		void copyBufferToImage(
			VkBuffer buffer,
			VkImage image,
			uint32_t width,
			uint32_t height);
		void transitionImageLayout(
			VkImage image,
			VkImageLayout oldLayout,
			VkImageLayout newLayout);

		VkShaderModule createShaderModule(const std::vector<char>& code);

		void waitIdle();

		std::vector<VkQueueFamilyProperties> queueFamilyProperties;

	 private:

		VksWindow& window;
		VkInstance instance;
		bool validationLayersEnabled;

		VkDebugUtilsMessengerEXT debugMessenger;

		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

		VkDevice device;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkQueue computeQueue;
		VkSurfaceKHR surface;
		VkCommandPool commandPool;

		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_MAINTENANCE1_EXTENSION_NAME
		};

		// Instancing
		void createInstance();

		void setupDebugMessenger();

		void createSurface();

		void pickPhysicalDevice();

		void createLogicalDevice();

		void createCommandPool();

		// Helper functions
		std::vector<const char*> getRequiredExtensions();

		bool checkValidationLayerSupport();

		VkResult
		CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger);

		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks* pAllocator);

		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		bool isDeviceSuitable(VkPhysicalDevice device);

		bool checkDeviceExtensionSupport(VkPhysicalDevice device);

		// Swapchain
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		VkPhysicalDeviceMemoryProperties memoryProperties;
	};

}

#endif //VULKANENGINE_VKS_DEVICE_H
