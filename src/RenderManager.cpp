//
// Created by magnias on 20/10/20.
//

#include "graphics/Vulkan.h"
#include "RenderManager.h"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <graphics/VulkanInitializers.h>


const std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
};

VKAPI_ATTR VkBool32 VKAPI_CALL
RenderManager::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                      VkDebugUtilsMessageTypeFlagsEXT messageType,
                      const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

RenderManager::RenderManager() : window(Window(800, 600)),
                                 vulkan(Vulkan(this->window.getGLFWwindow()))
{

    createInstance();
    vulkan.instance = instance;
    vulkan.setupDebugMessenger();
    vulkan.createSurface();
    vulkan.pickPhysicalDevice();
    vulkan.createLogicalDevice();
    vulkan.createSwapChain();
    vulkan.createImageViews();
    vulkan.createRenderPass();
    vulkan.createFramebuffers();
    vulkan.createCommandPool();

    //Createsyncobjects back
    std::cout << "Rendermanager initialized" << '\n';
}

RenderManager::~RenderManager()
{

}

void RenderManager::createInstance() {
    if (enableValidationLayers && !vulkan.checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = vulkan.getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        vks::initializers::populateDebugMessengerCreateInfo(debugCreateInfo, debugCallback);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}


void RenderManager::destroy()
{
    //TODO: Destroy vulkan
}


