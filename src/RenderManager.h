//
// Created by magnias on 20/10/20.
//

#include <graphics/Window.h>
#include <graphics/Vulkan.h>

class RenderManager
{
public:
    RenderManager();
    ~RenderManager();

    Window window;
    Vulkan vulkan;

    void destroy();

private:
    VkInstance instance;
    bool enableValidationLayers = true;

    void createInstance();

    static VkBool32
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);

};

