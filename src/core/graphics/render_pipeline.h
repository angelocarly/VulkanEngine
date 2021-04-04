//
// Created by magnias on 31/03/2021.
//

#ifndef VULKANENGINE_RENDER_PIPELINE_H
#define VULKANENGINE_RENDER_PIPELINE_H

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include "../../vks/vks_pipeline.h"
#include "../../vks/vks_swap_chain.h"
#include "../camera.h"

using namespace vks;

struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

struct MeshPushConstants
{
    glm::mat4 transform;
};

class IRenderPipeline
{
public:
    virtual ~IRenderPipeline()
    {};

    virtual void updateBuffers(int frame, Camera camera) = 0;
    virtual void bind(VkCommandBuffer commandBuffer, int frame) = 0;
    virtual void recreatePipeline() = 0;
    virtual VkPipelineLayout & getPipelineLayout() = 0;
};

class BaseRenderPipeline : public IRenderPipeline
{
public:
    BaseRenderPipeline(VksDevice &device, VksSwapChain &swapChain, VkDescriptorPool &descriptorPool)
            : _device(device), _descriptorPool(descriptorPool), _swapChain(swapChain)
    {
        init();
    }

    void init()
    {
        createDescriptorSetLayout();
        createPipelineLayout();
        createPipeline();
        createUniformBuffers();
        createDescriptorSets();
    }

    void bind(VkCommandBuffer commandBuffer, int frame) override
    {
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                                &descriptorSets[frame], 0, nullptr);


        pipeline->bind(commandBuffer);
    };

    VkPipelineLayout & getPipelineLayout() override
    {
        return pipelineLayout;
    }

    void updateBuffers(int frame, Camera camera) override
    {
        UniformBufferObject ubo{};
        ubo.model = camera.calculateModelMatrix();
        ubo.view = camera.calculateViewMatrix();
        ubo.proj = camera.calculateProjectionMatrix();
        ubo.proj[1][1] *= -1;

        ubo.view = glm::inverse(ubo.view);

        void *data;
        vkMapMemory(_device.getVkDevice(), _uniformBuffersMemory[frame], 0, sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(_device.getVkDevice(), _uniformBuffersMemory[frame]);
    }

    void recreatePipeline() override
    {
        createPipeline();
    }

private:

    VksDevice &_device;
    VkDescriptorPool &_descriptorPool;
    VksSwapChain &_swapChain;
    std::unique_ptr<VksPipeline> pipeline = nullptr;
    VkPipelineLayout pipelineLayout;
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkBuffer> _uniformBuffers;
    std::vector<VkDeviceMemory> _uniformBuffersMemory;

    /**
     * Create a pipeline layout
     * Stores the set of resources that can be used by the pipeline
     */
    void createPipelineLayout()
    {
//        spdlog::get("vulkan")->debug("Creating pipeline layout..");

        VkPushConstantRange push_constant;
        push_constant.offset = 0;
        push_constant.size = sizeof(MeshPushConstants);
        push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &push_constant;
        if (vkCreatePipelineLayout(_device.getVkDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }

    }

    void createPipeline()
    {
//        assert(_swapChain != nullptr && "Cannot create pipeline before swap chain");
//        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

//        spdlog::get("vulkan")->debug("Creating pipeline..");

        PipelineConfigInfo pipelineConfigInfo{};
        VksPipeline::defaultPipelineConfigInfo(pipelineConfigInfo);
        pipelineConfigInfo.renderPass = _swapChain.getRenderPass();
        pipelineConfigInfo.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<VksPipeline>(_device, _swapChain, pipelineConfigInfo);
    }

    /**
     * Stores the data that will be accessible to the descriptor set
     */
    void createDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;

        if (vkCreateDescriptorSetLayout(_device.getVkDevice(), &layoutInfo, nullptr, &descriptorSetLayout) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void createDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(_swapChain.getImageCount(), descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = _descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(_swapChain.getImageCount());
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(_swapChain.getImageCount());
        if (vkAllocateDescriptorSets(_device.getVkDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < _swapChain.getImageCount(); i++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = _uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;
            descriptorWrite.pImageInfo = nullptr; // Optional
            descriptorWrite.pTexelBufferView = nullptr; // Optional
            vkUpdateDescriptorSets(_device.getVkDevice(), 1, &descriptorWrite, 0, nullptr);
        }
    }

    void createUniformBuffers()
    {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        _uniformBuffers.resize(_swapChain.getImageCount());
        _uniformBuffersMemory.resize(_swapChain.getImageCount());

        for (size_t i = 0; i < _swapChain.getImageCount(); i++)
        {
            _device.createBuffer(
                    bufferSize,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    _uniformBuffers[i],
                    _uniformBuffersMemory[i]
            );
        }
    }
};

#endif //VULKANENGINE_RENDER_PIPELINE_H
