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

        ~VksSwapChain() { destroy(); }
        void destroy();

        VkExtent2D getSwapChainExtent() { return swapChainExtent; }
        VkRenderPass getRenderPass() { return renderPass; }

        int getImageCount() { return swapChainImages.size(); };
        VkFramebuffer getFrameBuffer(size_t i) { return swapChainFramebuffers.at(i); };
        VkResult acquireNextImage(uint32_t *imageIndex);
        VkResult submitCommandBuffers(VkCommandBuffer const *buffers, uint32_t *imageIndex);
        int getCurrentFrame() { return currentFrame; }
        std::vector<VkFence> getInFlightFences() { return inFlightFences; }

        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

        VkFormat getImagesFormat();

        void waitForImageInFlight();

        void recreate();

    private:
        VksWindow &window;
        VksDevice &device;

        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;

        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkFramebuffer> swapChainFramebuffers;

        VkRenderPass renderPass;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        size_t currentFrame = 0;

        // Instancing
        void init();
        void createSwapChain();
        void createImageViews();
        void createRenderPass();

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        void createFramebuffers();

        void createSyncObject();

        void recreateSwapChain();

        void cleanupSwapChain();
    };

}

#endif //VULKANENGINE_VKS_SWAP_CHAIN_H
