//
// Created by magnias on 20/10/20.
//

#ifndef VULKANENGINE_GAME_H
#define VULKANENGINE_GAME_H


#include <graphics/Window.h>
#include <graphics/Vulkan.h>
#include "RenderManager.h"

class Game : vks::Renderable
{
public:
    Game();
    void run();

private:
    RenderManager renderManager;
    void draw(VkCommandBuffer cmdbuffer, VkPipelineLayout pipelineLayout, int index) override;

    void update();

    void destroy();

    void createDescriptorPool(int size);

    void createDescriptorSets(int size);

    void createDescriptorSetLayout();

    void createUniformBuffers(int size);

    void updateUniformBuffer(uint32_t currentImage);
};


#endif //VULKANENGINE_GAME_H
