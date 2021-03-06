#ifndef VULKANENGINE_VKS_PIPELINE_H
#define VULKANENGINE_VKS_PIPELINE_H

#include <vector>
#include <string>
#include "vks_device.h"
#include "vks_swap_chain.h"

namespace vks
{

struct PipelineConfigInfo
{
//        VkViewport viewport;
//        VkRect2D scissor;
    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    std::vector<VkDynamicState> dynamicStateEnables;
    VkPipelineDynamicStateCreateInfo dynamicStateInfo;
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
};

/**
 * The pipeline specifies the pipeline that all graphics and commands pass trough in Vulkan.
 *
 * This can be a simple rasterization pipeline, a compute pipeline or others.
 */
class VksPipeline
{
public:

    VksPipeline(VksDevice &device, vks::VksSwapChain &swapChain, const PipelineConfigInfo &configInfo);

    ~VksPipeline()
    { destroy(); }

    void destroy();

//        static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);
    static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);

    VksPipeline(const VksPipeline &) = delete;

    void operator=(const VksPipeline &) = delete;

    void bind(VkCommandBuffer commandBuffer);

    VkPipeline getPipeline()
    { return graphicsPipeline; }

private:
    VksDevice &device;
    VksSwapChain &swapchain;

    PipelineConfigInfo _configInfo;
    VkPipeline graphicsPipeline;

    static std::vector<char> readFile(const std::__cxx11::basic_string<char> &filename);

    VkShaderModule_T *createShaderModule(const std::vector<char> &code);

    void createGraphicsPipeline(const PipelineConfigInfo &configInfo);

};

}

#endif //VULKANENGINE_VKS_PIPELINE_H
