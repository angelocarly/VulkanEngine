//
// Created by magnias on 23/03/2021.
//

#ifndef VULKANENGINE_RENDER_MANAGER_H
#define VULKANENGINE_RENDER_MANAGER_H

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include "../../vks/vks_model.h"

class IRenderProvider
{
public:
    virtual ~IRenderProvider(){};
    virtual void bindModelTransform(glm::mat4 transform) = 0;
    virtual void bindModel(vks::VksModel &model) = 0;
    virtual void drawModel() = 0;
};

class VulkanRenderManager: public IRenderProvider
{
public:

    struct MeshPushConstants
    {
        glm::mat4 transform;
    };

    VulkanRenderManager()
    {}

    void bindModelTransform(glm::mat4 transform) override
    {
        MeshPushConstants constants;
        constants.transform = transform;
        vkCmdPushConstants(*_commandBuffer, *_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0,
                           sizeof(MeshPushConstants), &constants);

    }

    void bindModel(vks::VksModel &model) override
    {
        _model = &model;
        _model->bind(*_commandBuffer);
    }

    void drawModel() override
    {
        _model->draw(*_commandBuffer);
    }

    void startFrame(VkCommandBuffer &commandBuffer, VkPipelineLayout &pipelineLayout)
    {
        _commandBuffer = &commandBuffer;
        _pipelineLayout = &pipelineLayout;
    }

private:
    VkCommandBuffer *_commandBuffer;
    VkPipelineLayout *_pipelineLayout;
    vks::VksModel *_model;
};

#endif //VULKANENGINE_RENDER_MANAGER_H
