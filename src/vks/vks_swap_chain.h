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
        VksSwapChain(VksDevice &device, VkExtent2D extent, bool vsync);
        VksSwapChain(VksDevice &device, VkExtent2D extent, bool vsync, std::shared_ptr<VksSwapChain> previous);

        ~VksSwapChain();

        VksSwapChain(const VksSwapChain &) = delete;
        VksSwapChain &operator=(const VksSwapChain &) = delete;

        void destroy();

        VkExtent2D getSwapChainExtent()
        { return swapChainExtent; }

        VkRenderPass getRenderPass()
        { return renderPass; }

        int getImageCount()
        { return swapChainImages.size(); };

        VkFramebuffer getFrameBuffer(size_t i)
        { return swapChainFramebuffers.at(i); };

        VkResult acquireNextImage(uint32_t *imageIndex);

        VkResult submitCommandBuffers(VkCommandBuffer const *buffers, uint32_t *imageIndex);

        VkFormat findDepthFormat();

        int getCurrentFrame()
        { return currentFrame; }

        std::vector<VkFence> getInFlightFences()
        { return inFlightFences; }

        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

        VkFormat getImagesFormat();

        void waitForImageInFlight();

        void recreate();

    private:
        VksDevice &device;

        bool vsync;

        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::shared_ptr<VksSwapChain> oldSwapChain;

        std::vector<VkImage> depthImages;
        std::vector<VkDeviceMemory> depthImageMemorys;
        std::vector<VkImageView> depthImageViews;
        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkFramebuffer> swapChainFramebuffers;

        VkRenderPass renderPass;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        size_t currentFrame = 0;

        // Instancing
        void init(VkExtent2D baseExtent);

        void createSwapChain(VkExtent2D baseExtent);

        void createImageViews();

        void createRenderPass();

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, VkExtent2D baseExtend);

        void createDepthResources();

        void createFramebuffers();

        void createSyncObject();

        void recreateSwapChain();

        void cleanupSwapChain();
    };

}

#endif //VULKANENGINE_VKS_SWAP_CHAIN_H
