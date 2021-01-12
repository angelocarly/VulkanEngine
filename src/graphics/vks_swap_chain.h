#ifndef VULKANENGINE_VKS_SWAP_CHAIN_H
#define VULKANENGINE_VKS_SWAP_CHAIN_H

#include <vector>
#include "vks_window.h"
#include "vks_device.h"

namespace vks
{

    class VksSwapChain
    {
    public:
        VksSwapChain(VksWindow &window, VksDevice &device);

        ~VksSwapChain();

    private:
        VksWindow &window;
        VksDevice &device;

        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
    };

}

#endif //VULKANENGINE_VKS_SWAP_CHAIN_H
